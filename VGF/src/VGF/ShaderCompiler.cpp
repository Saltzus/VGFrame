#include "ShaderCompiler.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <array>

#include "Log.h"
#include "nlohmann/json.hpp"
#include "Shader.h"

namespace VGF
{
	void ShaderCompiler::Init()
	{
		slang::createGlobalSession(globalSession.writeRef());

		slang::SessionDesc sessionDesc = {};

		slang::TargetDesc targetDesc = {};
		targetDesc.format = SLANG_SPIRV;
		targetDesc.profile = globalSession->findProfile("spirv_1_5");

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;
		sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;

		globalSession->createSession(sessionDesc, session.writeRef());

	}

	static void FillMemebers(ShaderReflection& reflection, slang::TypeLayoutReflection* typelayout, const std::string& prefix, std::unordered_map<std::string, MemberReflection>& out)
	{
		uint32_t fieldCount = static_cast<uint32_t>(typelayout->getFieldCount());
		for (size_t i = 0; i < fieldCount; i++)
		{
			slang::VariableLayoutReflection* field = typelayout->getFieldByIndex(i);
			slang::TypeLayoutReflection* fieldtype = field->getTypeLayout();

			std::string name = prefix + field->getName();

			MemberReflection member;
			member.offset = field->getOffset();
			member.size = fieldtype->getSize();
			member.parent = prefix;
			member.parent.pop_back();
			member.alignment = fieldtype->getAlignment();

			switch (fieldtype->getKind())
			{
			case slang::TypeReflection::Kind::Struct:
				FillMemebers(reflection, fieldtype, name + ".", member.children);
				break;
			case slang::TypeReflection::Kind::Array:
			{
				member.count = static_cast<uint32_t>(fieldtype->getElementCount());
				member.stride = fieldtype->getElementStride(SlangParameterCategory::SLANG_PARAMETER_CATEGORY_UNIFORM);
				member.size = member.count * member.stride;

				slang::TypeLayoutReflection* element = fieldtype->getElementTypeLayout();
				if (element->getKind() == slang::TypeReflection::Kind::Struct)
					FillMemebers(reflection, element, name + ".", member.children);
				break;
			}
			}

			out.emplace(name, std::move(member));
			reflection.memberLookup[name] = out[name];
		}
	}

	Slang::ComPtr<slang::IModule> ShaderCompiler::LoadModule(std::string path)
	{
		std::ostringstream sstream;
		std::ifstream fs(path);
		sstream << fs.rdbuf();
		const std::string str(sstream.str());
		const char* shaderData = str.c_str();

		Slang::ComPtr<slang::IModule> slangModule;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			const char* moduleName = path.c_str();
			const char* modulePath = path.c_str();
			slangModule = session->loadModuleFromSourceString(moduleName, modulePath, shaderData, diagnosticsBlob.writeRef());

			DiagnoseIfNeeded(diagnosticsBlob);
			if (!slangModule) SlangError("Could not load shader : " + path);
		}

		return slangModule;
	}

	ShaderReflection ShaderCompiler::LoadShader(Shader& shader, std::string path, std::string path2)
	{
		Slang::ComPtr<slang::IModule> module1 = LoadModule(path);
		Slang::ComPtr<slang::IModule> module2 = LoadModule(path2);

		Slang::ComPtr<slang::IEntryPoint> ePoint1, ePoint2;
		module1->getDefinedEntryPoint(0, ePoint1.writeRef());
		module2->getDefinedEntryPoint(0, ePoint2.writeRef());

		std::array<slang::IComponentType*, 4> componentTypes =
		{
			module1, module2,
			ePoint1, ePoint2
		};

		Slang::ComPtr<slang::IComponentType> composedProgram;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			SlangResult result = session->createCompositeComponentType(
				componentTypes.data(),
				componentTypes.size(),
				composedProgram.writeRef(),
				diagnosticsBlob.writeRef());

			DiagnoseIfNeeded(diagnosticsBlob);
			if (!composedProgram) SlangError("Error getting composed program");
		}

		Slang::ComPtr<slang::IComponentType> linkedProgram;
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			SlangResult result = composedProgram->link(
				linkedProgram.writeRef(),
				diagnosticsBlob.writeRef());

			DiagnoseIfNeeded(diagnosticsBlob);
			if (!linkedProgram) SlangError("Error getting linked program");
		}

		ShaderReflection reflection;

		slang::ProgramLayout* layout = linkedProgram->getLayout();
		slang::VariableLayoutReflection* globalVarLayout = layout->getGlobalParamsVarLayout();
		slang::TypeLayoutReflection* globalTypeLayout = globalVarLayout->getTypeLayout();

		for (size_t i = 0; i < layout->getEntryPointCount(); i++)
		{
			Slang::ComPtr<slang::IBlob> code;
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;

			SlangResult result = linkedProgram->getEntryPointCode
			(
				i, // entryPointIndex
				0, // targetIndex
				code.writeRef(),
				diagnosticsBlob.writeRef()
			);

			DiagnoseIfNeeded(diagnosticsBlob);
			if (!code) SlangError("Error getting spirv code");

			switch (layout->getEntryPointByIndex(i)->getStage())
			{
			case SLANG_STAGE_VERTEX: shader.spirvvert = code; break;
			case SLANG_STAGE_FRAGMENT: shader.spirvfrag = code; break;
			}
		}

		uint32_t fieldCount = globalTypeLayout->getFieldCount();
		for (uint32_t i = 0; i < fieldCount; i++)
		{
			slang::VariableLayoutReflection* fieldVarLayout = globalTypeLayout->getFieldByIndex(i);
			slang::TypeLayoutReflection* fieldTypeLayout = fieldVarLayout->getTypeLayout();

			uint32_t count = 1;
			if (fieldTypeLayout->getKind() == slang::TypeReflection::Kind::Array)
			{
				count = static_cast<uint32_t>(fieldTypeLayout->getElementCount());
				fieldTypeLayout = fieldTypeLayout->getElementTypeLayout();
			}

			BindingReflection binding;
			binding.binding = fieldVarLayout->getBindingIndex();
			binding.set = fieldVarLayout->getBindingSpace();
			binding.type = fieldTypeLayout->getType();
			binding.size = fieldTypeLayout->getElementTypeLayout()->getSize();
			binding.count = count;
			binding.alignment = fieldTypeLayout->getAlignment();
			binding.elementStride = fieldTypeLayout->getElementTypeLayout()->getStride();

			if (fieldTypeLayout->getKind() == slang::TypeReflection::Kind::ConstantBuffer)
			{
				slang::TypeLayoutReflection* inner = fieldTypeLayout->getElementTypeLayout();
				binding.members.clear();
				FillMemebers(reflection, inner, std::string(fieldVarLayout->getName()) + ".", binding.members);
			}

			reflection.sets[binding.set][fieldVarLayout->getName()] = binding;
			reflection.bindingLookup[fieldVarLayout->getName()] = binding;
		}

		for (uint32_t entryPointCount = 0; entryPointCount < layout->getEntryPointCount(); entryPointCount++)
		{
			Slang::ComPtr<slang::IMetadata> metadata;
			Slang::ComPtr<slang::IBlob> diagnostics;
			linkedProgram->getEntryPointMetadata(entryPointCount, 0, metadata.writeRef(), diagnostics.writeRef());
			if (!metadata) continue;

			SlangStage stage = layout->getEntryPointByIndex(entryPointCount)->getStage();

			for (auto& [set, bindings] : reflection.sets)
			{
				for (auto& [name, binding] : bindings) {
					bool used = false;
					metadata->isParameterLocationUsed(SLANG_PARAMETER_CATEGORY_DESCRIPTOR_TABLE_SLOT, binding.set, binding.binding, used);
					if (used) binding.stages.push_back(stage);
				}
			}
		}

		return reflection;
	}

	void ShaderCompiler::SlangError(std::string text)
	{
		std::cout << "\033[38;5;202m[SLANG]\033[0m  " << text << "\n";
		throw std::runtime_error(text);
	}

	void ShaderCompiler::DiagnoseIfNeeded(slang::IBlob* diagnosticsBlob)
	{
		if (diagnosticsBlob != nullptr)
		{
			SlangError((const char*)diagnosticsBlob->getBufferPointer());
		}
	}

}


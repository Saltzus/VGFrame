#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

namespace VGF
{
	struct MemberReflection
	{
		std::string parent;
		uint32_t offset = 0;
		uint32_t size = 0;
		uint32_t alignment = 0;
		uint32_t stride = 0;
		uint32_t count = 1;
		std::unordered_map<std::string, MemberReflection> children;
	};

	struct BindingReflection
	{
		uint32_t set, binding, count = 1;
		size_t size = 0;
		uint32_t alignment = 0;
		uint32_t elementStride = 0;
		std::vector<SlangStage> stages;
		slang::TypeReflection* type;
		std::unordered_map<std::string, MemberReflection> members;
	};

	struct StageReflection
	{
		std::string entry;
	};

	struct VertexReflection
	{
		std::string name;
		uint32_t location;
	};

	struct ShaderReflection
	{
		std::vector<StageReflection> stages;
		std::map<uint32_t, std::unordered_map<std::string,BindingReflection>> sets;
		std::vector<VertexReflection> vertex;
		std::unordered_map<std::string, MemberReflection> memberLookup;
		std::unordered_map<std::string, BindingReflection> bindingLookup;
	};

	class Shader;
	class ShaderCompiler
	{
	public:
		static void Init();
		static ShaderReflection LoadShader(Shader&, std::string, std::string);

	private:
		static Slang::ComPtr<slang::IModule> LoadModule(std::string path);
		static void SlangError(std::string);
		static void DiagnoseIfNeeded(slang::IBlob* diagnosticsBlob);

		static inline Slang::ComPtr<slang::IGlobalSession> globalSession;
		static inline Slang::ComPtr<slang::ISession> session;
	};
}

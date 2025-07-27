#include "Model.h"

namespace VGF
{
	Model::Model(const char* modelPath)
	{
		bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, modelPath);
		if (!warn.empty())
			printf("Warn: %s\n", warn.c_str());
		if (!err.empty())
			printf("Err: %s\n", err.c_str());
		if (!ret)
			printf("Failed to parse glTF\n");
		else
			std::cout << "\nSuccesfully parsed gltf" << "  \n";

		textures = createTextureObjects(model);

		for (size_t i = 0; i < model.meshes.size(); i++)
		{
			Mesh mesh;
			LoadMeshData(model, model.meshes[i], mesh.vertices, mesh.indices);
			meshes.push_back(mesh);
		}

		std::cout << "Model Loaded Succesfully\n";

	}

	Model::Model(std::vector <float> vertices, std::vector <unsigned int> indices, Material* material)
	{
		customModel = true;

		Mesh mesh;
		mesh.vertices = vertices;
		mesh.indices = indices;

		meshes.push_back(mesh);
	}

	Model::~Model()
	{
		delete matrixBuffer;
		delete pbrBuffer;
		delete lightBuffer;
	}

	/// Read flat floats from an accessor
	static const float* GetFloatData(const tinygltf::Model& model, int accessorId) 
	{
		const tinygltf::Accessor& accessor = model.accessors[accessorId];
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

		return reinterpret_cast<const float*>
		(
			buffer.data.data() + bufferView.byteOffset + accessor.byteOffset
		);
	}

	/// Read flat unsigned ints from an accessor (for indices)
	static const unsigned short* GetUShortData(const tinygltf::Model& model, int accessorId) 
	{
		const tinygltf::Accessor& accessor = model.accessors[accessorId];
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

		return reinterpret_cast<const unsigned short*>
		(
			buffer.data.data() + bufferView.byteOffset + accessor.byteOffset
		);
	}

	void Model::LoadMeshData
	(
		const tinygltf::Model& model,
		tinygltf::Mesh& mesh,
		std::vector<float>& outVertices,
		std::vector<uint32_t>& outIndices
	)
	{
		assert(!model.meshes.empty());
		for (const auto& prim : mesh.primitives) 
		{
			if (prim.indices >= 0) 
			{
				const tinygltf::Accessor& indiceAccessor = model.accessors[prim.indices];
				size_t indicesCount = indiceAccessor.count;

				switch (indiceAccessor.componentType)
				{
				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: 
				{
					auto data = GetUShortData(model, prim.indices);

					for (size_t i = 0; i < indicesCount; ++i)
						outIndices.push_back(static_cast<uint32_t>(data[i]));

				} break;

				case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: 
				{
					const auto* data = reinterpret_cast<const uint32_t*>
					(
						model.buffers[model.bufferViews[indiceAccessor.bufferView].buffer].data.data()
						+ model.bufferViews[indiceAccessor.bufferView].byteOffset
						+ indiceAccessor.byteOffset
					);

					for (size_t i = 0; i < indicesCount; ++i)
						outIndices.push_back(data[i]);

				} break;

				default:
					assert(false && "Unsupported index component type");
				}
			}

			const auto positionAttribute = prim.attributes.find("POSITION");
			const auto normalAtribute = prim.attributes.find("NORMAL");
			const auto uvAttribute = prim.attributes.find("TEXCOORD_0");

			assert(positionAttribute != prim.attributes.end());
			const int positionAccessor = positionAttribute->second;
			const float* positionData = GetFloatData(model, positionAccessor);
			size_t vertCount = model.accessors[positionAccessor].count;

			const float* normalData = nullptr;
			if (normalAtribute != prim.attributes.end())
				normalData = GetFloatData(model, normalAtribute->second);

			const float* uvData = nullptr;
			if (uvAttribute != prim.attributes.end())
				uvData = GetFloatData(model, uvAttribute->second);

			outVertices.clear();
			outVertices.reserve(vertCount * 11);

			for (size_t vert = 0; vert < vertCount; ++vert) 
			{

				const float* position = positionData + 3 * vert;
				float positionX = position[0], positionY = position[1], positionZ = position[2];

				// read normal (or default to zero)
				float normalX = 0.0f, normalY = 0.0f, normalZ = 0.0f;
				if (normalData) 
				{
					const float* normal = normalData + 3 * vert;
					normalX = normal[0]; normalY = normal[1]; normalZ = normal[2];
				}

				// read uv (or default to zero)
				float uvU = 0.0f, uvV = 0.0f;
				if (uvData) 
				{
					const float* uv = uvData + 2 * vert;
					uvU = uv[0];
					uvV = uv[1];
				}

				// push 8 floats in order: pos.xyz, normal.xyz, uv.xy
				outVertices.push_back(positionX);
				outVertices.push_back(positionY);
				outVertices.push_back(positionZ);

				outVertices.push_back(normalX);
				outVertices.push_back(normalY);
				outVertices.push_back(normalZ);

				outVertices.push_back(1);
				outVertices.push_back(1);
				outVertices.push_back(1);

				outVertices.push_back(uvU);
				outVertices.push_back(1.0f - uvV);
			}
		}
	}
	std::vector<Texture*> Model::createTextureObjects(const tinygltf::Model& model) const
	{
		std::vector<Texture*> textureObjects;

		//tinygltf::Sampler defaultSampler;
		//defaultSampler.minFilter = GL_LINEAR;
		//defaultSampler.magFilter = GL_LINEAR;
		//defaultSampler.wrapS = GL_REPEAT;
		//defaultSampler.wrapT = GL_REPEAT;
		////defaultSampler.wrapR = GL_REPEAT;

		for (size_t i = 0; i < model.textures.size(); i++)
		{
			const auto& texture = model.textures[i];
			assert(texture.source >= 0);
			const auto& image = model.images[texture.source];
			

			textureObjects.emplace_back(new Texture(image.image.data(), GL_RGBA, image.width, image.height));
		}

		return textureObjects;
	}

	glm::mat4 Model::getLocalToWorldMatrix(const tinygltf::Node& node, const glm::mat4& parentMatrix)
	{
		if (!node.matrix.empty()) {
			return parentMatrix * glm::mat4(node.matrix[0], node.matrix[1],
				node.matrix[2], node.matrix[3], node.matrix[4],
				node.matrix[5], node.matrix[6], node.matrix[7],
				node.matrix[8], node.matrix[9], node.matrix[10],
				node.matrix[11], node.matrix[12], node.matrix[13],
				node.matrix[14], node.matrix[15]);
		}
		const auto T = node.translation.empty() ? parentMatrix :
			glm::translate(parentMatrix, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));

		const auto rotationQuat = node.rotation.empty() ? glm::quat(1, 0, 0, 0) : glm::quat(float(node.rotation[3]), float(node.rotation[0]),
			float(node.rotation[1]), float(node.rotation[2]));

		const auto TR = T * glm::mat4_cast(rotationQuat);
		return node.scale.empty() ? TR : glm::scale(TR, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
	};



	void Model::drawNodes(std::vector<Renderer*> renderers, int nodeIdx, const glm::mat4& parentMatrix, PipelineConfig& config, Camera* camera, LightBufferObject lightBufffer)
	{
		const auto& node = model.nodes[nodeIdx];
		const glm::mat4 modelMatrix = getLocalToWorldMatrix(node, parentMatrix);
		const auto& mesh = model.meshes[node.mesh];

		matrixBuffer->data.model = parentMatrix;
		matrixBuffer->data.proj = camera->projection;
		matrixBuffer->data.view = camera->view;

		matrixBuffer->data.proj[1][1] *= -1;

		std::vector<UniformBufferObject*> uniformBuffers;
		uniformBuffers.push_back(matrixBuffer);
		uniformBuffers.push_back(bindMaterial(camera, mesh.primitives[0].material));
		uniformBuffers.push_back(lightBuffer->getDefault());

		if (node.mesh >= 0)
		{
			Renderer* renderer = renderers[node.mesh];
			renderer->Render(config, uniformBuffers);
		}
		
		for (const auto childNodeIdx : node.children) 
			drawNodes(renderers, childNodeIdx, modelMatrix, config, camera, *lightBuffer);
	};

	void Model::renderModel(std::vector<Renderer*> renderers, const glm::mat4& parentMatrix, PipelineConfig& config, Camera* camera)
	{
		if (customModel)
			for (size_t i = 0; i < meshes.size(); i++)
			{
				matrixBuffer->data.model = parentMatrix;
				matrixBuffer->data.proj = camera->projection;
				matrixBuffer->data.view = camera->view;

				matrixBuffer->data.proj[1][1] *= -1;

				std::vector<UniformBufferObject*> uniformBuffers;
				uniformBuffers.push_back(matrixBuffer);
				uniformBuffers.push_back(bindMaterial(camera, -1));
				uniformBuffers.push_back(lightBuffer->getDefault());


				renderers[i]->Render(config, uniformBuffers);
			}

		else
			for (const auto nodeIdx : model.scenes[model.defaultScene].nodes)
				drawNodes(renderers, nodeIdx, parentMatrix, config, camera, *lightBuffer);
	}

	PBRbufferObject* Model::bindMaterial(Camera* camera, const int materialIndex)
	{
		pbrBuffer->data.cameraPosition = camera->Position;

		if (materialIndex >= 0)
		{
			const auto& modelMaterial = model.materials[materialIndex];
			const auto& pbrMetallicRoughness = modelMaterial.pbrMetallicRoughness;

			pbrBuffer->data.baseColorFactor =
			{
				(float)pbrMetallicRoughness.baseColorFactor[0],
				(float)pbrMetallicRoughness.baseColorFactor[1],
				(float)pbrMetallicRoughness.baseColorFactor[2],
				(float)pbrMetallicRoughness.baseColorFactor[3]
			};


			Texture* textureObject = Texture::GetDefaultTexture();
			if (pbrMetallicRoughness.baseColorTexture.index >= 0)
			{
				const auto& texture = model.textures[pbrMetallicRoughness.baseColorTexture.index];
				if (texture.source >= 0)
				{
					textureObject = textures[texture.source];
				}
			}
			textureObject->Bind(textureType::color);


			pbrBuffer->data.metallicFactor = (float)pbrMetallicRoughness.metallicFactor;

			pbrBuffer->data.roughnessFactor = (float)pbrMetallicRoughness.roughnessFactor;


			textureObject = Texture::GetWhiteTexture();
			if (pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
			{
				const auto& texture = model.textures[pbrMetallicRoughness.metallicRoughnessTexture.index];
				if (texture.source >= 0)
				{
					textureObject = textures[texture.source];
				}
			}
			textureObject->Bind(textureType::metallicRoughness);

			pbrBuffer->data.emissiveFactor =
			{
				(float)modelMaterial.emissiveFactor[0],
				(float)modelMaterial.emissiveFactor[1],
				(float)modelMaterial.emissiveFactor[2]
			};


			textureObject = Texture::GetBlackTexture();
			if (modelMaterial.emissiveTexture.index >= 0)
			{
				const auto& texture = model.textures[modelMaterial.emissiveTexture.index];
				if (texture.source >= 0)
				{
					textureObject = textures[texture.source];
				}
			}
			textureObject->Bind(textureType::emissive);


			pbrBuffer->data.occlusionStrength = (float)modelMaterial.occlusionTexture.strength;


			textureObject = Texture::GetWhiteTexture();
			if (modelMaterial.occlusionTexture.index >= 0)
			{
				const auto& texture = model.textures[modelMaterial.occlusionTexture.index];
				
				if (texture.source >= 0)
				{
					textureObject = textures[texture.source];
				}
			}
			textureObject->Bind(textureType::occulsion);

			textureObject = Texture::GetBlueTexture();
			if (modelMaterial.normalTexture.index >= 0)
			{
				const auto& texture = model.textures[modelMaterial.normalTexture.index];

				if (texture.source >= 0)
				{
					textureObject = textures[texture.source];
				}
			}
			textureObject->Bind(textureType::normal);
		}
		else
		{
			std::vector<Texture*> text = Material::getDefaultMaterial()->getVector();
			text[0]->Bind(textureType::color);
			text[1]->Bind(textureType::metallicRoughness);
			text[2]->Bind(textureType::emissive);
			text[3]->Bind(textureType::occulsion);
			text[4]->Bind(textureType::normal);
		}

		return pbrBuffer;
	};
}


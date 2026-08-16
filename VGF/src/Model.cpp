#include "Model.h"

namespace VGF
{
	Model::Model(const std::string modelPath, const PipelineConfig& config, const std::vector<UniformBufferObject*> additionalUniformBuffers)
		: config(config)
	{
		tinygltf::Model gltfModel;
		tinygltf::TinyGLTF loader;
		std::string err, warn;

		bool ret = false;
		std::string extension = modelPath.substr(modelPath.find_last_of(".") + 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

		if (extension == "glb")
			ret = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, modelPath);
		//else if (extension == "gltf")
		//	ret = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, modelPath);
		else
			err = "Unsupported file extension: " + extension + ". Expected .gltf or .glb";

		if (!warn.empty())
			std::cout << "glTF warning: " << warn << std::endl;
		if (!err.empty())
			std::cout << "glTF error: " << err << std::endl;
		if (!ret)
			throw std::runtime_error("Failed to load glTF model");

		textures = createTextureObjects(model);

		for (size_t i = 0; i < gltfModel.nodes.size(); i++)
		{
			const auto& node = gltfModel.nodes[i];
			linearNodes[i] = new Node();
			linearNodes[i]->index = static_cast<uint32_t>(i);
			linearNodes[i]->name = node.name;

			if (node.translation.size() == 3)
				linearNodes[i]->translation = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
			if (node.rotation.size() == 4)
				linearNodes[i]->rotation = glm::quat(node.rotation[0], node.rotation[1], node.rotation[2], node.rotation[3]);
			if (node.translation.size() == 3)
				linearNodes[i]->scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
		}

		for (size_t i = 0; i < gltfModel.nodes.size(); i++)
		{
			const auto& node = gltfModel.nodes[i];
			for (int childIdx : node.children)
			{
				linearNodes[childIdx]->parent = linearNodes[i];
				linearNodes[i]->children.push_back(linearNodes[childIdx]);
			}
		}

		for (size_t i = 0; i < gltfModel.nodes.size(); i++)
		{
			const auto& node = gltfModel.nodes[i];
			if (node.mesh >= 0)
			{
				const auto& mesh = gltfModel.meshes[node.mesh];
				for (const auto& primitive : mesh.primitives)
				{
					Mesh newMesh;

					if (primitive.material >= 0) newMesh.materialIndex = primitive.material;

					LoadMeshData(model, model.meshes[i], newMesh.vertices, newMesh.indices);
					linearNodes[i]->mesh = newMesh;
				}
			}
		}

		for (const auto& anim : gltfModel.animations) {
			Animation animation;
			animation.name = anim.name;

			for (const auto& sampler : anim.samplers)
			{
				AnimationSampler animSampler{};

				if (sampler.interpolation == "LINEAR") animSampler.interpolation = AnimationSampler::LINEAR;
				else if (sampler.interpolation == "STEP") animSampler.interpolation = AnimationSampler::STEP;
				else if (sampler.interpolation == "CUBICSPLINE") animSampler.interpolation = AnimationSampler::CUBICSPLINE;

				{
					const tinygltf::Accessor &  accessor   = input.accessors[glTFSampler.input];
					const tinygltf::BufferView &bufferView = input.bufferViews[accessor.bufferView];
					const tinygltf::Buffer &    buffer     = input.buffers[bufferView.buffer];
					const void *                dataPtr    = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
					const float *               buf        = static_cast<const float *>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++)
					{
						dstSampler.inputs.push_back(buf[index]);
					}
					// Adjust animation's start and end times
					for (auto input : animations[i].samplers[j].inputs)
					{
						if (input < animations[i].start)
						{
							animations[i].start = input;
						};
						if (input > animations[i].end)
						{
							animations[i].end = input;
						}
					}

				animation.samplers.push_back(animSampler);
			}

			// Connect samplers to node properties
			for (const auto& channel : anim.channels) {
				AnimationChannel animChannel{};

				// Set target node and property (translation, rotation, or scale)
				// ... (code omitted for brevity)

				animation.channels.push_back(animChannel);
			}

			model.animations.push_back(animation);
		}

		std::vector<UniformBufferObject*> renderUniformBuffers;
		renderUniformBuffers.insert(renderUniformBuffers.end(), uniformBuffers.begin(), uniformBuffers.end());
		renderUniformBuffers.insert(renderUniformBuffers.end(), additionalUniformBuffers.begin(), additionalUniformBuffers.end());

		for (auto mesh : meshes)
			modelRenderers.emplace_back(new Renderer(mesh.indices, mesh.vertices, renderUniformBuffers));

		std::cout << "Model Loaded Succesfully\n";
	}

	Model::Model(std::vector <float> vertices, std::vector <unsigned int> indices, Material* material, const PipelineConfig& config, const std::vector<UniformBufferObject*> additionalUniformBuffers) 
		: config(config)
	{
		_material = material;
		customModel = true;

		Mesh mesh;
		mesh.vertices = vertices;
		mesh.indices = indices;

		meshes.push_back(mesh);

		std::vector<UniformBufferObject*> renderUniformBuffers;
		renderUniformBuffers.insert(renderUniformBuffers.end(), uniformBuffers.begin(), uniformBuffers.end());
		renderUniformBuffers.insert(renderUniformBuffers.end(), additionalUniformBuffers.begin(), additionalUniformBuffers.end());


		for (auto mesh : meshes)
			modelRenderers.emplace_back(new Renderer(mesh.indices, mesh.vertices, renderUniformBuffers));
	}

	Model::~Model()
	{
		for (auto& uniform : uniformBuffers)
			delete uniform;

		for (auto& renderer : modelRenderers)
			delete renderer;
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
			uint32_t firstIndex = static_cast<uint32_t>(outIndices.size());
			uint32_t vertexStart = static_cast<uint32_t>(outVertices.size());
			uint32_t indexCount = 0;
			bool hasSkin = false;

			const float* positionBuffer = nullptr;
			const float* normalsBuffer = nullptr;
			const float* texCoordsBuffer = nullptr;
			const uint16_t* jointIndicesBuffer = nullptr;
			const float* jointWeightsBuffer = nullptr;
			size_t vertexCount = 0;

			// Get buffer data for vertex normals
			if (prim.attributes.contains("POSITION"))
			{
				const tinygltf::Accessor &  accessor = model.accessors[prim.attributes.find("POSITION")->second];
				const tinygltf::BufferView &view     = model.bufferViews[accessor.bufferView];
				positionBuffer                       = reinterpret_cast<const float *>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				vertexCount                          = accessor.count;
			}
			// Get buffer data for vertex normals
			if (prim.attributes.contains("NORMAL"))
			{
				const tinygltf::Accessor &  accessor = model.accessors[prim.attributes.find("NORMAL")->second];
				const tinygltf::BufferView &view     = model.bufferViews[accessor.bufferView];
				normalsBuffer                        = reinterpret_cast<const float *>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}
			// Get buffer data for vertex texture coordinates
			if (prim.attributes.contains("TEXCOORD_0"))
			{
				const tinygltf::Accessor &  accessor = model.accessors[prim.attributes.find("TEXCOORD_0")->second];
				const tinygltf::BufferView &view     = model.bufferViews[accessor.bufferView];
				texCoordsBuffer                      = reinterpret_cast<const float *>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			// Get vertex joint indices
			if (prim.attributes.contains("JOINTS_0"))
			{
				const tinygltf::Accessor &  accessor = model.accessors[prim.attributes.find("JOINTS_0")->second];
				const tinygltf::BufferView &view     = model.bufferViews[accessor.bufferView];
				jointIndicesBuffer                   = reinterpret_cast<const uint16_t *>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}
			// Get vertex joint weights
			if (prim.attributes.contains("WEIGHTS_0"))
			{
				const tinygltf::Accessor &  accessor = model.accessors[prim.attributes.find("WEIGHTS_0")->second];
				const tinygltf::BufferView &view     = model.bufferViews[accessor.bufferView];
				jointWeightsBuffer                   = reinterpret_cast<const float *>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			hasSkin = (jointIndicesBuffer && jointWeightsBuffer);

			// Append data to model's vertex buffer
			std::vector<DefaultVertex> vertices;
			for (size_t v = 0; v < vertexCount; v++)
			{
				DefaultVertex vert{};
				vert.position     = glm::make_vec3(&positionBuffer[v * 3]);
				vert.normal       = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
				vert.texCoord     = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec2(0.0f);
				vert.color        = glm::vec3(1.0f);
				vert.jointIndices = hasSkin ? glm::vec4(glm::make_vec4(&jointIndicesBuffer[v * 4])) : glm::vec4(0.0f);
				vert.jointWeights = hasSkin ? glm::make_vec4(&jointWeightsBuffer[v * 4]) : glm::vec4(0.0f);
				vertices.push_back(vert);
			}

			const float* raw = reinterpret_cast<const float*>(vertices.data());
			outVertices = std::vector<float>(raw, raw + vertices.size() * 19);

			// Indices

			const tinygltf::Accessor&  accessor    = model.accessors[prim.indices];
			const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer&    buffer      = model.buffers[bufferView.buffer];

			indexCount += static_cast<uint32_t>(accessor.count);

			switch (accessor.componentType)
			{
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
					const uint32_t* buf = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
					for (size_t index = 0; index < accessor.count; index++)
					{
						outIndices.push_back(buf[index] + vertexStart);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
					const uint16_t* buf = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
					for (size_t index = 0; index < accessor.count; index++)
					{
						outIndices.push_back(buf[index] + vertexStart);
					}
					break;
				}
				case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
					const uint8_t* buf = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + bufferView.byteOffset]);
					for (size_t index = 0; index < accessor.count; index++)
					{
						outIndices.push_back(buf[index] + vertexStart);
					}
					break;
				}
				default:
					std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
					return;
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

	std::vector<UniformBufferObject*> Model::AddUniformBuffers(const glm::mat4& parentMatrix, const int bindId, const Camera& camera, std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		MatrixData data;
		data.model = parentMatrix;
		data.proj = camera.projection;
		data.view = camera.view;
		uniformBuffers[matrixBuffer]->SetData((void*)&data);

		std::vector<UniformBufferObject*> renderUniformBuffers;
		renderUniformBuffers.push_back(uniformBuffers[matrixBuffer]);
		renderUniformBuffers.push_back(bindMaterial(camera, bindId));
		renderUniformBuffers.push_back(LightBufferObject::getDefault());
		renderUniformBuffers.insert(renderUniformBuffers.end(), additionalUniformBuffers.begin(), additionalUniformBuffers.end());

		return renderUniformBuffers;
	}

	void Model::Draw(const glm::mat4& parentMatrix, const Renderer* renderer, const int bindId, const PipelineConfig& config, const Camera& camera, const std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		std::vector<UniformBufferObject*> renderUniformBuffers = AddUniformBuffers(parentMatrix, bindId, camera, additionalUniformBuffers);
		renderer->Render(config, renderUniformBuffers);
	}

	void Model::BatchDraw(const glm::mat4& parentMatrix, const Renderer* renderer, const int bindId, const PipelineConfig& config, const Camera& camera, const void* instanceData, size_t instanceCount, size_t instanceStride, std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		std::vector<UniformBufferObject*> renderUniformBuffers = AddUniformBuffers(parentMatrix, bindId, camera, additionalUniformBuffers);
		renderer->BatchRender(config, instanceData, instanceCount, instanceStride, renderUniformBuffers);
	}

	void Model::drawNodes(int nodeIdx, const glm::mat4& parentMatrix, const PipelineConfig& config, const Camera& camera, LightBufferObject lightBufffer)
	{
		const auto& node = model.nodes[nodeIdx];
		const glm::mat4 modelMatrix = getLocalToWorldMatrix(node, parentMatrix);
		const auto& mesh = model.meshes[node.mesh];

		if (node.mesh >= 0)
		{
			Renderer* renderer = modelRenderers[node.mesh];
			Draw(parentMatrix, renderer, mesh.primitives[0].material, config, camera);
		}
		
		for (const auto childNodeIdx : node.children) 
			drawNodes(childNodeIdx, modelMatrix, config, camera, *LightBufferObject::getDefault());
	};



	void Model::Render(const glm::mat4& parentMatrix, const Camera& camera, const PipelineConfig& config, const std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		if (customModel) {
			for (size_t i = 0; i < meshes.size(); i++){
				Draw(parentMatrix, modelRenderers[i], -1, config, camera, additionalUniformBuffers);
			}
		}
		else {
			for (const auto nodeIdx : model.scenes[model.defaultScene].nodes) {
				drawNodes(nodeIdx, parentMatrix, config, camera, *LightBufferObject::getDefault());
			}
		}
	}

	void Model::BatchRender(const glm::mat4& parentMatrix, const Camera& camera, const PipelineConfig& config, const void* instanceData, size_t instanceCount, size_t instanceStride, std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		if (!customModel) return;
		for (size_t i = 0; i < meshes.size(); i++) 
		{
			BatchDraw(parentMatrix, modelRenderers[i], -1, config, camera, instanceData, instanceCount, instanceStride, additionalUniformBuffers);
		}
	}


	UniformBufferObject* Model::bindMaterial(const Camera& camera, const int materialIndex)
	{
		PBRData pbrBufferData = *(static_cast<const PBRData*>(uniformBuffers[pbrBuffer]->Data()));
		pbrBufferData.cameraPosition = camera.position;

		if (_material != nullptr) _material->Bind();
		else if (materialIndex >= 0)
		{
			const auto& modelMaterial = model.materials[materialIndex];
			const auto& pbrMetallicRoughness = modelMaterial.pbrMetallicRoughness;

			pbrBufferData.baseColorFactor =
			{
				(float)pbrMetallicRoughness.baseColorFactor[0],
				(float)pbrMetallicRoughness.baseColorFactor[1],
				(float)pbrMetallicRoughness.baseColorFactor[2],
				(float)pbrMetallicRoughness.baseColorFactor[3]
			};

			const Texture* textureObject = Texture::GetWhiteTexture();

			if (pbrMetallicRoughness.baseColorTexture.index >= 0)
			{
				const auto& texture = model.textures[pbrMetallicRoughness.baseColorTexture.index];
				if (texture.source >= 0)
				{
					textureObject = textures[texture.source];
				}
			}
			textureObject->Bind(textureType::color);


			pbrBufferData.metallicFactor = (float)pbrMetallicRoughness.metallicFactor;

			pbrBufferData.roughnessFactor = (float)pbrMetallicRoughness.roughnessFactor;


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

			pbrBufferData.emissiveFactor =
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


			pbrBufferData.occlusionStrength = (float)modelMaterial.occlusionTexture.strength;


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
			std::vector<const Texture*> text = Material::getDefaultMaterial()->getVector();
			text[0]->Bind(textureType::color);
			text[1]->Bind(textureType::metallicRoughness);
			text[2]->Bind(textureType::emissive);
			text[3]->Bind(textureType::occulsion);
			text[4]->Bind(textureType::normal);
		}

		uniformBuffers[pbrBuffer]->SetData((void*)&pbrBufferData);
		return uniformBuffers[pbrBuffer];
	};
}


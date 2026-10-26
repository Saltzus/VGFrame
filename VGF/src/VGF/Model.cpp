#include "Model.h"

#include <cmath>

#include "Physics.h"
#include "VGFTime.h"

namespace VGF
{
	Model::Model(const std::string& modelPath, const Shader& config, const std::vector<UniformBufferObject*> additionalUniformBuffers)
		: config(config)
	{
		tinygltf::Model gltfModel;
		if (auto it = gltfModels.find(modelPath); it != gltfModels.end())
		{
			gltfModel = it->second;
		}
		else
		{
			tinygltf::TinyGLTF loader;
			std::string err, warn;

			bool ret = false;
			std::string extension = modelPath.substr(modelPath.find_last_of(".") + 1);
			std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

			if (extension == "glb")
				ret = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, modelPath);
			else if (extension == "gltf")
				ret = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, modelPath);
			else
				err = "Unsupported file extension: " + extension + ". Expected .gltf or .glb";

			if (!warn.empty())
				std::cout << "glTF warning: " << warn << std::endl;
			if (!err.empty())
				std::cout << "glTF error: " << err << std::endl;
			if (!ret)
				throw std::runtime_error("Failed to load glTF model");
		}
		
		_textures = createTextureObjects(gltfModel);

		// add nodes.
		for (size_t i = 0; i < gltfModel.nodes.size(); i++)
		{
			const auto& node = gltfModel.nodes[i];
			nodes.push_back(new Node);
			nodes[i]->index = static_cast<uint32_t>(i);
			nodes[i]->name = node.name;
			nodes[i]->skin = node.skin;

			if (node.translation.size() == 3)
				nodes[i]->translation = glm::vec3(node.translation[0], node.translation[1], node.translation[2]);
			if (node.rotation.size() == 4)
				nodes[i]->rotation = glm::quat(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
			if (node.scale.size() == 3)
				nodes[i]->scale = glm::vec3(node.scale[0], node.scale[1], node.scale[2]);
		}

		// add node hierarchy.
		for (size_t i = 0; i < gltfModel.nodes.size(); i++)
		{
			const auto& node = gltfModel.nodes[i];
			for (int childIdx : node.children)
			{
				nodes[childIdx]->parent = nodes[i];
				nodes[i]->children.push_back(nodes[childIdx]);
			}
		}

		// add node meshes.
		for (size_t i = 0; i < gltfModel.nodes.size(); i++)
		{
			const auto& node = gltfModel.nodes[i];
			if (node.mesh >= 0)
			{
				const auto& mesh = gltfModel.meshes[node.mesh];

				Mesh newMesh;
				for (const auto& primitive : mesh.primitives)
					if (primitive.material >= 0) newMesh.materialIndex = primitive.material;

				LoadMeshData(gltfModel, gltfModel.meshes[node.mesh], newMesh.vertices, newMesh.indices);
				nodes[i]->mesh = newMesh;
			}
		}

		for (const auto& anim : gltfModel.animations) 
		{
			Animation animation;
			animation.name = anim.name;

			for (const auto& sampler : anim.samplers)
			{
				AnimationSampler animSampler{};

				if (sampler.interpolation == "LINEAR") animSampler.interpolation = AnimationSampler::LINEAR;
				else if (sampler.interpolation == "STEP") animSampler.interpolation = AnimationSampler::STEP;
				else if (sampler.interpolation == "CUBICSPLINE") animSampler.interpolation = AnimationSampler::CUBICSPLINE;

				animSampler.interpolation = AnimationSampler::LINEAR;
				//if (animSampler.interpolation == AnimationSampler::STEP || animSampler.interpolation == AnimationSampler::CUBICSPLINE)
					//VGF::Log::Error("SET or CUBICSPLINE not implemented");

				{
					const tinygltf::Accessor&  accessor = gltfModel.accessors[sampler.input];
					const tinygltf::BufferView& bufferView = gltfModel.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = gltfModel.buffers[bufferView.buffer];
					const void* dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];
					const float* buf = static_cast<const float *>(dataPtr);
					for (size_t index = 0; index < accessor.count; index++)
					{
						animSampler.inputs.push_back(buf[index]);
					}

					for (auto input : animSampler.inputs)
					{
						if (input < animation.start) animation.start = input;
						if (input > animation.end) animation.end = input;
					}
				}

				{
					const tinygltf::Accessor &accessor = gltfModel.accessors[sampler.output];
					const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
					const tinygltf::Buffer &buffer = gltfModel.buffers[bufferView.buffer];

					assert(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT);

					const void *dataPtr = &buffer.data[accessor.byteOffset + bufferView.byteOffset];

					switch (accessor.type) 
					{
					case TINYGLTF_TYPE_VEC3: 
					{
						const glm::vec3 *buf = static_cast<const glm::vec3*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
							animSampler.outputsVec3.push_back(buf[index]);
						break;
					}
					case TINYGLTF_TYPE_VEC4: 
					{
						const glm::vec4 *buf = static_cast<const glm::vec4*>(dataPtr);
						for (size_t index = 0; index < accessor.count; index++)
							animSampler.outputsVec4.push_back(buf[index]);
						break;
					}
					default: 
					{
						std::cout << "unknown type" << std::endl;
						break;
					}
					}
				}

				animation.samplers.push_back(animSampler);
			}


			for (const auto& channel : anim.channels) {
				AnimationChannel animChannel{};

				if(channel.target_path == "rotation")		animChannel.path = AnimationChannel::ROTATION;
				if(channel.target_path == "translation")	animChannel.path = AnimationChannel::TRANSLATION;
				if(channel.target_path == "scale")			animChannel.path = AnimationChannel::SCALE;
				if(channel.target_path == "weights")		animChannel.path;

				animChannel.samplerIndex = channel.sampler;
				animChannel.node         = nodes[channel.target_node];

				animation.channels.push_back(animChannel);
			}

			animations.push_back(animation);
		}

		for (size_t i = 0; i < gltfModel.skins.size(); i++)
		{
			tinygltf::Skin glTFSkin = gltfModel.skins[i];

			skins.emplace_back();
			skins[i].name = glTFSkin.name;


			// Find joint nodes
			for (int jointIndex : glTFSkin.joints)
			{
				Node *node = nodes[jointIndex];
				if (node)
				{
					if (!node->parent || std::ranges::find(glTFSkin.joints, node->parent->index) == glTFSkin.joints.end())
						skins[i].skeletonRoot = nodes[jointIndex];

					skins[i].joints.push_back(node);
				}
			}

			// Get the inverse bind matrices from the buffer associated to this skin
			if (glTFSkin.inverseBindMatrices > -1)
			{
				const tinygltf::Accessor &  accessor   = gltfModel.accessors[glTFSkin.inverseBindMatrices];
				const tinygltf::BufferView &bufferView = gltfModel.bufferViews[accessor.bufferView];
				const tinygltf::Buffer &    buffer     = gltfModel.buffers[bufferView.buffer];
				memcpy(skins[i].inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufferView.byteOffset], accessor.count * sizeof(glm::mat4));

				AnimationData data;
				std::copy(skins[i].inverseBindMatrices.begin(), skins[i].inverseBindMatrices.end(), data.jointMatrices);
				renderData.Set("jointMatrices", data.jointMatrices);
			}
		}

		textures = gltfModel.textures;
		materials = gltfModel.materials;

		std::vector<UniformBufferObject*> renderUniformBuffers;
		renderUniformBuffers.insert(renderUniformBuffers.end(), uniformBuffers.begin(), uniformBuffers.end());
		renderUniformBuffers.insert(renderUniformBuffers.end(), additionalUniformBuffers.begin(), additionalUniformBuffers.end());

		for (auto& node : nodes)
		{
			if (node->mesh.indices.empty()) continue;
			modelRenderers.emplace_back(new Renderer(Shader::GetDefault(), node->mesh.indices, node->mesh.vertices));
			node->mesh.rendererIndex = modelRenderers.size()-1;
		}
	}

	Model::Model(std::vector <float> vertices, std::vector <unsigned int> indices, Material* material, const Shader& config, const std::vector<UniformBufferObject*> additionalUniformBuffers) 
		: config(config)
	{
		_material = material;
		customModel = true;

		Mesh mesh;
		mesh.vertices = vertices;
		mesh.indices = indices;

		nodes.push_back(new Node());
		nodes[0]->index = static_cast<uint32_t>(0);
		nodes[0]->name = "unnamed";
		nodes[0]->mesh = mesh;


		std::vector<UniformBufferObject*> renderUniformBuffers;
		renderUniformBuffers.insert(renderUniformBuffers.end(), uniformBuffers.begin(), uniformBuffers.end());
		renderUniformBuffers.insert(renderUniformBuffers.end(), additionalUniformBuffers.begin(), additionalUniformBuffers.end());


		for (auto& node : nodes)
		{
			for (auto node : nodes) continue;
			modelRenderers.emplace_back(new Renderer(Shader::GetDefault(), node->mesh.indices, node->mesh.vertices));
			node->mesh.rendererIndex = modelRenderers.size()-1;
		}
	}

	Model::~Model()
	{
		for (auto& uniform : uniformBuffers)	delete uniform;
		for (auto& renderer : modelRenderers)	delete renderer;
		for (auto& node : nodes)			delete node;
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

	void Model::UpdateAnimation(Animation& animation, float deltaTime)
	{
        animation.currentTime += deltaTime * animation.speed;

        if (animation.currentTime > animation.end)
			if (animation.looping) animation.currentTime = animation.start;

        for (auto& channel : animation.channels)
        {
	        AnimationSampler& sampler = animation.samplers[channel.samplerIndex];

        	auto keyFrameIt = std::ranges::lower_bound(sampler.inputs, animation.currentTime);
        	if (keyFrameIt != sampler.inputs.end() && keyFrameIt != sampler.inputs.begin())
        	{
        		size_t indx = std::distance(sampler.inputs.begin(), keyFrameIt) - 1;
        		float factor = (animation.currentTime - sampler.inputs[indx]) / (sampler.inputs[indx + 1] - sampler.inputs[indx]);

        		switch (channel.path)
        		{
        		case AnimationChannel::TRANSLATION:
				{
        				glm::vec3 start = sampler.outputsVec3[indx];
        				glm::vec3 end = sampler.outputsVec3[indx + 1];
        				channel.node->translation = glm::mix(start, end, factor);
        				break;
        		}
        		case AnimationChannel::ROTATION:
				{
        				glm::quat start = glm::quat(sampler.outputsVec4[indx].w, sampler.outputsVec4[indx].x, sampler.outputsVec4[indx].y, sampler.outputsVec4[indx].z);
        				glm::quat end = glm::quat(sampler.outputsVec4[indx + 1].w, sampler.outputsVec4[indx + 1].x, sampler.outputsVec4[indx + 1].y, sampler.outputsVec4[indx + 1].z);
        				channel.node->rotation = glm::slerp(start, end, factor);
        				break;
        		}
        		case AnimationChannel::SCALE:
				{
        				glm::vec3 start = sampler.outputsVec3[indx];
        				glm::vec3 end = sampler.outputsVec3[indx + 1];
        				channel.node->scale = glm::mix(start, end, factor);
        				break;
        		}
        		}
        	}
        }
	}

	void Model::UpdateJoints(Node* node)
	{
		if (node->skin > -1)
		{
			glm::mat4 inverseTransform = glm::inverse((node->GetGlobalMatrix()));
			Skin& skin = skins[node->skin];
			size_t numJoints = (uint32_t) skin.joints.size();
			std::vector<glm::mat4> jointMatrices(numJoints);
			for (size_t i = 0; i < numJoints; i++)
			{
				jointMatrices[i] = skin.joints[i]->GetGlobalMatrix() * skin.inverseBindMatrices[i];
				jointMatrices[i] = inverseTransform * jointMatrices[i];
			}

			AnimationData data;
			std::copy(jointMatrices.begin(), jointMatrices.end(), data.jointMatrices);

			renderData.Set("jointMatrices", data.jointMatrices);
		}

		for (auto &child : node->children)
		{
			UpdateJoints(child);
		}
	}

	void Model::PlayAnimation(size_t index, float speed)
	{
		selectedAnimation = &animations[index];
		selectedAnimation->speed = speed;
		selectedAnimation->currentTime = 0.f;
	}

	void Model::BlendAnimations(Animation& fromAnimation, Animation& toAnimation, float blendFactor) {
		// Store original node transformations
		std::vector<glm::vec3> originalTranslations;
		std::vector<glm::quat> originalRotations;
		std::vector<glm::vec3> originalScales;

		for (const Node* node : nodes)
		{
			originalTranslations.push_back(node->translation);
			originalRotations.push_back(node->rotation);
			originalScales.push_back(node->scale);
		}

		// Apply first animation fully
		UpdateAnimation(fromAnimation, 0.0f);

		// Store intermediate transformations
		std::vector<glm::vec3> fromTranslations;
		std::vector<glm::quat> fromRotations;
		std::vector<glm::vec3> fromScales;

		for (const Node* node : nodes)
		{
			fromTranslations.push_back(node->translation);
			fromRotations.push_back(node->rotation);
			fromScales.push_back(node->scale);
		}

		// Restore original transformations
		for (size_t i = 0; i < nodes.size(); i++)
		{
			nodes[i]->translation = originalTranslations[i];
			nodes[i]->rotation = originalRotations[i];
			nodes[i]->scale = originalScales[i];
		}

		// Apply second animation fully
		UpdateAnimation(toAnimation, 0.0f);

		// Blend between the two animations
		for (size_t i = 0; i < nodes.size(); i++)
		{
			nodes[i]->translation = glm::mix(fromTranslations[i], nodes[i]->translation, blendFactor);
			nodes[i]->rotation = glm::slerp(fromRotations[i], nodes[i]->rotation, blendFactor);
			nodes[i]->scale = glm::mix(fromScales[i], nodes[i]->scale, blendFactor);
		}
	}

	bool Model::SolveTwoBoneIK
	(
    	Node* rootNode,      // The root joint (e.g., shoulder or hip)
    	Node* midNode,       // The middle joint (e.g., elbow or knee)
    	Node* endNode,       // The end effector (e.g., hand or foot)
    	const glm::vec3& targetPosition,  // Target world position
    	const glm::vec3& poleVector,       // Axis of rotation for the middle joint
    	float preferredAngle      // Preferred angle for resolving ambiguity
	)
	{

		glm::vec3 rootPosition = glm::vec3(rootNode->GetGlobalMatrix()[3]);
		glm::vec3 midPosition = glm::vec3(midNode->GetGlobalMatrix()[3]);
		glm::vec3 endPosition = glm::vec3(endNode->GetGlobalMatrix()[3]);

		float lenght1 = glm::length(midPosition - rootPosition);
		float lenght2 = glm::length(endPosition - midPosition);
		if (lenght1 < 0.00001f || lenght2 < 0.00001f) return false;


		glm::vec3 toTarget = targetPosition - rootPosition;
		float distance = glm::length(toTarget);
		if (distance < 0.00001f) return false;
		
		glm::vec3 targetDirection = toTarget / distance;

		float minReach = std::abs(lenght1 - lenght2) + 0.0001f;
		float maxReach = lenght1 + lenght2 - 0.0001f;
		float clampedDistance = glm::clamp(distance, minReach, maxReach);
		glm::vec3 clampedTarget = rootPosition + targetDirection * clampedDistance;


		float cosA = (clampedDistance * clampedDistance + lenght1 * lenght1 - lenght2 * lenght2) / (2.0f * clampedDistance * lenght1);
		cosA = glm::clamp(cosA, -1.f, 1.f);
		float angleAtRoot = std::acos(cosA);

		glm::vec3 forward = targetDirection;
		glm::vec3 up = poleVector - forward * glm::dot(poleVector, forward);
		if (glm::dot(up, up) < 0.000001f)
		{
			glm::vec3 altPole = (std::abs(forward.x) < 0.9f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
			up = altPole - forward * glm::dot(altPole, forward);
		}
		up = glm::normalize(up);

		glm::vec3 desiredMidDirection = glm::cos(angleAtRoot) * forward + glm::sin(angleAtRoot) * up;
		glm::vec3 desiredMidPosition = rootPosition + desiredMidDirection * lenght1;

		glm::vec3 rootBoneDir = glm::normalize(desiredMidPosition - rootPosition);
		glm::vec3 rootRollDir = up;
		rootRollDir = glm::normalize(rootRollDir - rootBoneDir * glm::dot(rootRollDir, rootBoneDir));
		glm::vec3 rootRight = glm::cross(rootBoneDir, rootRollDir);

		glm::mat3 rootBasis;
		rootBasis[0] = rootRight;
		rootBasis[1] = rootBoneDir;
		rootBasis[2] = rootRollDir;

		glm::quat rootWorldNew = glm::normalize(glm::quat_cast(rootBasis));

		glm::quat parentWorldRot = rootNode->parent
			? glm::normalize(glm::quat_cast(glm::mat3(rootNode->parent->GetGlobalMatrix())))
			: glm::quat(1, 0, 0, 0);

		rootNode->rotation = glm::normalize(glm::inverse(parentWorldRot) * rootWorldNew);


		glm::vec3 midBoneDir = glm::normalize(clampedTarget - desiredMidPosition);
		glm::vec3 midRollDir = up;
		midRollDir = glm::normalize(midRollDir - midBoneDir * glm::dot(midRollDir, midBoneDir));
		if (glm::dot(midRollDir, midRollDir) < 1e-6f) {
			glm::vec3 alt = (std::abs(midBoneDir.y) < 0.9f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
			midRollDir = glm::normalize(alt - midBoneDir * glm::dot(alt, midBoneDir));
		}
		glm::vec3 midRight = glm::cross(midBoneDir, midRollDir);

		glm::mat3 midBasis;
		midBasis[0] = midRight;
		midBasis[1] = midBoneDir;
		midBasis[2] = midRollDir;

		glm::quat midWorldNew = glm::normalize(glm::quat_cast(midBasis));

		midNode->rotation = glm::normalize(glm::inverse(rootWorldNew) * midWorldNew);

		return true;
	}

	void Model::ApplyJointConstraints(Node* node, const glm::vec3& minAngles, const glm::vec3& maxAngles)
	{
		// Convert quaternion to Euler angles
		glm::vec3 eulerAngles = glm::degrees(glm::eulerAngles(node->rotation));

		// Apply constraints
		eulerAngles.x = glm::clamp(eulerAngles.x, minAngles.x, maxAngles.x);
		eulerAngles.y = glm::clamp(eulerAngles.y, minAngles.y, maxAngles.y);
		eulerAngles.z = glm::clamp(eulerAngles.z, minAngles.z, maxAngles.z);

		// Convert back to quaternion
		glm::quat constrainedRotation = glm::quat(glm::radians(eulerAngles));

		// Apply the constrained rotation
		node->rotation = constrainedRotation;
	}

	// Apply IK on top of an animation
	void Model::ApplyIKToAnimation(Animation& animation, float deltaTime, Node* endEffector, const glm::vec3& targetPosition, float ikWeight) {
		// First, update the animation normally
		UpdateAnimation(animation, deltaTime);

		// If IK weight is zero, we're done
		if (ikWeight <= 0.0f) return;

		// Build the joint chain from end effector to root
		std::vector<Node*> chain;
		Node* current = endEffector;

		// Add up to 3 joints to the chain (e.g., hand, elbow, shoulder)
		while (current && chain.size() < 3)
		{
			chain.push_back(current);
			current = current->parent;
		}

		// Reverse the chain to go from root to end effector
		std::ranges::reverse(chain);

		// Store original rotations
		std::vector<glm::quat> originalRotations;
		for (const Node* node : chain)
			originalRotations.push_back(node->rotation);

		// Apply IK
		//SolveTwoBoneIK(chain[0], chain[1], chain[2], targetPosition,
		//			  glm::vec3(0.0f, 0.0f, 1.0f));

		// Blend between original and IK rotations based on weight
		if (ikWeight < 1.0f)
		{
			for (size_t i = 0; i < chain.size(); i++)
			{
				chain[i]->rotation = glm::slerp(originalRotations[i],chain[i]->rotation,ikWeight);
			}
		}
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
			uint32_t vertexStart = static_cast<uint32_t>(outVertices.size()/19);
			uint32_t indexCount = 0;
			bool hasSkin = false;

			const float* positionBuffer = nullptr;
			const float* normalsBuffer = nullptr;
			const float* texCoordsBuffer = nullptr;
			const uint8_t* jointIndicesBuffer = nullptr;
			const float* jointWeightsBuffer = nullptr;
			size_t vertexCount = 0;

			// Get buffer data for vertex normals
			if (prim.attributes.contains("POSITION"))
			{
				const tinygltf::Accessor& accessor = model.accessors[prim.attributes.find("POSITION")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
				positionBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
				vertexCount = accessor.count;
			}
			// Get buffer data for vertex normals
			if (prim.attributes.contains("NORMAL"))
			{
				const tinygltf::Accessor& accessor = model.accessors[prim.attributes.find("NORMAL")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
				normalsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}
			// Get buffer data for vertex texture coordinates
			if (prim.attributes.contains("TEXCOORD_0"))
			{
				const tinygltf::Accessor& accessor = model.accessors[prim.attributes.find("TEXCOORD_0")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
				texCoordsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			// Get vertex joint indices
			if (prim.attributes.contains("JOINTS_0"))
			{
				const tinygltf::Accessor& accessor = model.accessors[prim.attributes.find("JOINTS_0")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
				jointIndicesBuffer = &(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]);
			}
			// Get vertex joint weights
			if (prim.attributes.contains("WEIGHTS_0"))
			{
				const tinygltf::Accessor& accessor = model.accessors[prim.attributes.find("WEIGHTS_0")->second];
				const tinygltf::BufferView& view = model.bufferViews[accessor.bufferView];
				jointWeightsBuffer = reinterpret_cast<const float*>(&(model.buffers[view.buffer].data[accessor.byteOffset + view.byteOffset]));
			}

			hasSkin = (jointIndicesBuffer && jointWeightsBuffer);


			// Append data to model's vertex buffer
			std::vector<DefaultVertex> vertices;
			vertices.reserve(vertexCount);

			for (size_t v = 0; v < vertexCount; v++)
			{
				DefaultVertex vert{};
				vert.position = glm::make_vec3(&positionBuffer[v * 3]);
				vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
				vert.texCoord = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec2(0.0f);
				vert.color = glm::vec3(1.0f);
				vert.jointIndices = hasSkin ? glm::vec4
				(
					(float)jointIndicesBuffer[v * 4 + 0],
					(float)jointIndicesBuffer[v * 4 + 1],
					(float)jointIndicesBuffer[v * 4 + 2],
					(float)jointIndicesBuffer[v * 4 + 3]
				) : glm::vec4(0.0f);
				vert.jointWeights = hasSkin ? glm::make_vec4(&jointWeightsBuffer[v * 4]) : glm::vec4(0.0f);
				vertices.push_back(vert);
			}

			const float* raw = reinterpret_cast<const float*>(vertices.data());
			outVertices.insert(outVertices.end(), raw, raw + vertices.size() * 19);

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

		for (size_t i = 0; i < model.textures.size(); i++)
		{
			const auto& texture = model.textures[i];
			assert(texture.source >= 0);
			const auto& image = model.images[texture.source];

			textureObjects.emplace_back(new Texture(image.image.data(), GL_RGBA, image.width, image.height));
		}

		return textureObjects;
	}

	glm::mat4 Model::getLocalToWorldMatrix(const Node& node, const glm::mat4& parentMatrix)
	{
		return glm::mat4(1.f);
	};

	void Model::AddRenderData(const glm::mat4& parentMatrix, const int bindId, const Camera& camera, std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		renderData.Set("MatrixBuffer.model", parentMatrix);
		renderData.Set("MatrixBuffer.proj", camera.projection);
		renderData.Set("MatrixBuffer.view", camera.view);

		renderData.Set("LightBuffer.ambientLightColor", glm::vec3(1.f, 1.f, 1.f));
		renderData.Set("LightBuffer.ambientLightIntensity", 0.5f);

		struct DirectionalLight
		{
			alignas(16)glm::vec3 direction;
			alignas(16)glm::vec3 color;
		};

		struct PointLight
		{
			alignas(16)glm::vec3 position;
			alignas(16)glm::vec3 color;
		};

		std::vector<DirectionalLight> directionalLights;
		directionalLights.resize(30);
		
		directionalLights[0].direction = glm::vec3(0, 0.5f, 0.5f);
		directionalLights[0].color = glm::vec3(5, 0, 0);
		directionalLights[1].direction = glm::vec3(0, -0.5f, -0.5f);
		directionalLights[1].color = glm::vec3(0, 0, 5);

		renderData.Set("LightBuffer.directionalLights", directionalLights);
		renderData.Set("LightBuffer.directionLightAmount", 2);

		std::vector<PointLight> pointLights;
		pointLights.resize(30);

		pointLights[0].position = glm::vec3(0, 10, 0);
		pointLights[0].color = glm::vec3(0.9, 0.9, 1);

		renderData.Set("LightBuffer.pointLights", pointLights);
		renderData.Set("LightBuffer.pointLightAmount", 1);

		bindMaterial(camera, bindId);
		
		DefaultInstance instance;
		instance.model = parentMatrix;
		instance.textureID = 1;

		renderData.instanceData.Append(&instance, 1, sizeof(DefaultInstance));
	}

	void Model::Draw(const glm::mat4& parentMatrix, const Renderer* renderer, const int bindId, const Shader& shader, const Camera& camera, const std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		AddRenderData(parentMatrix, bindId, camera, additionalUniformBuffers);
		renderer->Render(renderData, shader);
	}

	void Model::BatchDraw(const glm::mat4& parentMatrix, const Renderer* renderer, const int bindId, const Shader& shader, const Camera& camera, const void* instanceData, size_t instanceCount, size_t instanceStride, std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		AddRenderData(parentMatrix, bindId, camera, additionalUniformBuffers);
		renderer->BatchRender(renderData, shader);
	}

	void Model::drawNodes(int nodeIdx, const glm::mat4& parentMatrix, const Shader& shader, const Camera& camera, LightBufferObject lightBufffer)
	{
		const auto& node = nodes[nodeIdx];
		const glm::mat4 modelMatrix = parentMatrix * node->GetLocalMatrix();
		const auto& mesh = node->mesh;

		if (node->skin != -1)
		{
			if (uniformBuffers[animationBuffer]) delete uniformBuffers[animationBuffer];
			uniformBuffers[animationBuffer] = new AnimationBufferObject(skins[node->skin].buffers[0]);
		}
			
		if (node->mesh.rendererIndex != -1)
		{
			Renderer* renderer = modelRenderers[node->mesh.rendererIndex];
			Draw(modelMatrix, renderer, mesh.materialIndex, config, camera);
		}
		
		for (const auto childNode : node->children)
		{
			drawNodes(childNode->index, modelMatrix, config, camera, *LightBufferObject::getDefault());
		}

	};



	void Model::Render(const glm::mat4& parentMatrix, const Camera& camera, const Shader& config, const std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		if (customModel)
		{
			for (size_t i = 0; i < nodes.size(); i++)
			{
				Draw(parentMatrix, modelRenderers[i], -1, config, camera, additionalUniformBuffers);
			}
		}
		else
		{
			if (selectedAnimation) UpdateAnimation(*selectedAnimation, VGF::Time::GetDeltaTime());
	
			for (auto &node : nodes)
			{
				if (node->parent == nullptr)
					UpdateJoints(node);
			}
			
			for (const auto node : nodes)
			{
				if (node->parent != nullptr) continue;
				drawNodes(node->index, parentMatrix, config, camera, *LightBufferObject::getDefault());
			}
		}
	}

	void Model::BatchRender(const glm::mat4& parentMatrix, const Camera& camera, const Shader& config, const void* instanceData, size_t instanceCount, size_t instanceStride, std::vector<UniformBufferObject*> additionalUniformBuffers)
	{
		if (!customModel) return;
		for (size_t i = 0; i < nodes.size(); i++)
		{
			BatchDraw(parentMatrix, modelRenderers[i], -1, config, camera, instanceData, instanceCount, instanceStride, additionalUniformBuffers);
		}
	}


	void Model::bindMaterial(const Camera& camera, const int materialIndex)
	{
		renderData.Set("PBRBuffer.cameraPosition", camera.position);
		renderData.Set("PBRBuffer.baseColorFactor", glm::vec4(1.f));
		renderData.Set("PBRBuffer.metallicFactor", 1.f);
		renderData.Set("PBRBuffer.roughnessFactor", 1.f);
		renderData.Set("PBRBuffer.emissiveFactor", glm::vec3(0.f));
		renderData.Set("PBRBuffer.occlusionStrength", 1.f);

		if (_material != nullptr) _material->Bind();
		else if (materialIndex >= 0)
		{
			const auto& modelMaterial = materials[materialIndex];
			const auto& pbrMetallicRoughness = modelMaterial.pbrMetallicRoughness;

			glm::vec4 colorFactor;
			colorFactor.x = pbrMetallicRoughness.baseColorFactor[0];
			colorFactor.y = pbrMetallicRoughness.baseColorFactor[1];
			colorFactor.z = pbrMetallicRoughness.baseColorFactor[2];
			colorFactor.w = pbrMetallicRoughness.baseColorFactor[3];

			renderData.Set("PBRBuffer.baseColorFactor", colorFactor);

			const Texture* textureObject = Texture::GetWhiteTexture();

			if (pbrMetallicRoughness.baseColorTexture.index >= 0)
			{
				const auto& texture = textures[pbrMetallicRoughness.baseColorTexture.index];
				if (texture.source >= 0)
				{
					textureObject = _textures[texture.source];
				}
			}
			textureObject->Bind(textureType::color);

			renderData.Set("PBRBuffer.metallicFactor", (float)pbrMetallicRoughness.metallicFactor);
			renderData.Set("PBRBuffer.roughnessFactor", (float)pbrMetallicRoughness.roughnessFactor);

			textureObject = Texture::GetWhiteTexture();
			if (pbrMetallicRoughness.metallicRoughnessTexture.index >= 0)
			{
				const auto& texture = textures[pbrMetallicRoughness.metallicRoughnessTexture.index];
				if (texture.source >= 0)
				{
					textureObject = _textures[texture.source];
				}
			}
			textureObject->Bind(textureType::metallicRoughness);

			glm::vec4 emissiveFactor;
			colorFactor.x = modelMaterial.emissiveFactor[0];
			colorFactor.y = modelMaterial.emissiveFactor[1];
			colorFactor.z = modelMaterial.emissiveFactor[2];

			renderData.Set("PBRBuffer.emissiveFactor", emissiveFactor);

			textureObject = Texture::GetBlackTexture();
			if (modelMaterial.emissiveTexture.index >= 0)
			{
				const auto& texture = textures[modelMaterial.emissiveTexture.index];
				if (texture.source >= 0)
				{
					textureObject = _textures[texture.source];
				}
			}
			textureObject->Bind(textureType::emissive);

			renderData.Set("PBRBuffer.occlusionStrength", (float)modelMaterial.occlusionTexture.strength);

			textureObject = Texture::GetWhiteTexture();
			if (modelMaterial.occlusionTexture.index >= 0)
			{
				const auto& texture = textures[modelMaterial.occlusionTexture.index];
				
				if (texture.source >= 0)
				{
					textureObject = _textures[texture.source];
				}
			}
			textureObject->Bind(textureType::occulsion);

			textureObject = Texture::GetBlueTexture();
			if (modelMaterial.normalTexture.index >= 0)
			{
				const auto& texture = textures[modelMaterial.normalTexture.index];

				if (texture.source >= 0)
				{
					textureObject = _textures[texture.source];
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
	};
}


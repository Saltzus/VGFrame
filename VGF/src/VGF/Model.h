#pragma once

#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <vector>

#include "Material.h"
#include "Renderer.h"
#include "UniformBuffers/AnimationBuffer.h"
#include "RenderData.h"

#include <glm/gtx/matrix_decompose.hpp>

#include "UniformBuffers/MatrixBuffer.h"
#include "UniformBuffers/LightBuffer.h"
#include "UniformBuffers/PBRBuffer.h"

namespace VGF
{
	struct Mesh
	{
		int rendererIndex = -1;
		std::vector<float> vertices;
		std::vector<unsigned int> indices;
		int materialIndex = -1;
	};

	struct Node
	{
		std::string name;
		unsigned int index;

		Node* parent = nullptr;
		std::vector<Node*> children;
		Mesh mesh;

		glm::mat4 matrix = glm::mat4(1.f);

		//Animation
		glm::vec3 translation = glm::vec3(0.f);
		glm::quat rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
		glm::vec3 scale = glm::vec3(1.0f);
		int skin = -1;

		glm::mat4 GetLocalMatrix() const
		{
			return glm::translate(glm::mat4(1.f), translation) *
			glm::toMat4(rotation) *
			glm::scale(glm::mat4(1.f), scale);
		}

		glm::mat4 GetGlobalMatrix() const
		{
			if (parent) return parent->GetGlobalMatrix() * GetLocalMatrix();
			return GetLocalMatrix();
		}
	};

	struct Skin
	{
		std::string name;
		Node *skeletonRoot = nullptr;
		std::array<glm::mat4,128> inverseBindMatrices;
		std::vector<Node *> joints;

		std::array<AnimationBufferObject, 2> buffers;
	};

	struct AnimationChannel
	{
		enum PathType { TRANSLATION, ROTATION, SCALE };
		PathType path;
		Node* node = nullptr;
		uint32_t samplerIndex;
	};

	struct AnimationSampler
	{
		enum InterpolationType { LINEAR, STEP, CUBICSPLINE };
		InterpolationType interpolation;
		std::vector<float> inputs;
		std::vector<glm::vec4> outputsVec4;
		std::vector<glm::vec3> outputsVec3;
	};

	struct Animation
	{
		std::string name;
		std::vector<AnimationSampler> samplers;
		std::vector<AnimationChannel> channels;
		float start = std::numeric_limits<float>::max();
		float end = std::numeric_limits<float>::min();
		float currentTime = 0.0f;
		bool looping = false;
		float speed = 1.f;
	};


	class Model
	{
	public:
		Model(const std::string& modelPath, const Shader& config);
		Model(std::vector <float> vertices, std::vector <unsigned int> indices, Material* material, const Shader& config);

		Model(const std::string modelPath)
			: Model(modelPath, VGF::Shader::GetDefault()) {}

		Model(std::vector <float> vertices, std::vector <unsigned int> indices, Material* material)
			: Model(vertices, indices, material, VGF::Shader::GetDefault()) {
		}
		Model(std::vector <float> vertices, std::vector <unsigned int> indices)
			: Model(vertices, indices, VGF::Material::getDefaultMaterial()) {}

		~Model();

		Shader config;
		RenderData renderData = RenderData(config);

		static inline std::unordered_map<std::string, tinygltf::Model> gltfModels;

		std::vector<Node*> nodes;
		std::vector<tinygltf::Material> materials;
		std::vector<tinygltf::Texture> textures;

		std::vector<Animation> animations;
		std::vector<Skin> skins;

		Node* FindNode(const std::string& name) {
			auto nodeIt = std::ranges::find_if(nodes, [&name](auto const& node)
			{
				return node->name == name;
			});
			return (nodeIt != nodes.end()) ? *nodeIt : nullptr;
		}

		void PlayAnimation(size_t index, bool clearTime = true, float speed = 1);
		void PlayAnimation(std::string name, bool clearTime = true, float speed = 1)
		{
			for (size_t index = 0; index < animations.size(); index++)
			{
				if (animations[index].name == name)
				{
					PlayAnimation(index, clearTime, speed);
					return;
				}
			}

			Log::Error("Could not find animation = " + name);
		};

		void StopAnimation() 
		{ 
			if (selectedAnimation)
			{
				selectedAnimation->looping = false;
				selectedAnimation->currentTime = 1.f;
			}
		}

		void BlendAnimations(Animation& fromAnimation, Animation& toAnimation, float blendFactor);

		static bool SolveTwoBoneIK
		(
			Node* rootNode,
			Node* midNode,
			Node* endNode,
			const glm::vec3& targetPosition,
			const glm::vec3& hingeAxis,
			float preferredAngle = 0.0f
		);

		static void ApplyJointConstraints(Node* node, const glm::vec3& minAngles, const glm::vec3& maxAngles);
		void ApplyIKToAnimation(Animation& animation, float deltaTime, Node* endEffector, const glm::vec3& targetPosition, float ikWeight = 1.0f);

		void Render(const glm::mat4& parentMatrix, const Camera& camera, const Shader& config);

		bool isCustomModel() { return customModel; }

	private:
		std::vector<Renderer*> modelRenderers;
		Material* _material = nullptr;

		bool customModel = false;

		struct VaoRange
		{
			int begin;
			int count;
		};

		std::vector <Texture*> _textures;
		Animation* selectedAnimation = nullptr;

		unsigned int uLightDirectionLocation;
		unsigned int uLightIntensity;

		unsigned int uBaseColorTexture = 1;
		unsigned int uBaseColorFactor;

		unsigned int uMetallicRoughnessTexture;
		unsigned int uMetallicFactor;

		unsigned int uRoughnessFactor;

		unsigned int uEmissiveTexture;
		unsigned int uEmissiveFactor;

		unsigned int uOcclusionTexture;
		unsigned int uOcclusionStrength;

		unsigned int uApplyOcclusion;

		unsigned int overrideTexture = 0;

		std::vector<unsigned int> vertexArrayObjects;
		std::vector<VaoRange> meshToVertexArrays;

		void UpdateJoints(Node* node);
		void UpdateAnimation(Animation& animation, float deltaTime);
		void Draw(const glm::mat4& parentMatrix, const Renderer* renderer, const int bindMaterial, const Shader& config, const Camera& camera, const std::vector<UniformBufferObject*> additionalUniformBuffers = {});
		void BatchDraw(const glm::mat4& parentMatrix, const Renderer* renderer, const int bindMaterial, const Shader& config, const Camera& camera, const void* instanceData, size_t instanceCount, size_t instanceStride, std::vector<UniformBufferObject*> additionalUniformBuffers = {});
		void drawNodes(int nodeIdx, const glm::mat4& parentMatrix, const Shader& config, const Camera& camera, LightBufferObject lightBuffer);

		void AddModelRenderData();
		void AddNodeRenderData(const glm::mat4& parentMatrix, const int bindId, const Camera& camera);

		static void LoadMeshData
		(
			const tinygltf::Model& model,
			tinygltf::Mesh& mesh,
			std::vector<float>& outVertices,
			std::vector<uint32_t>& outIndices
		);

		std::vector<Texture*> createTextureObjects(const tinygltf::Model& model) const;
		glm::mat4 getLocalToWorldMatrix(const Node& node, const glm::mat4& parentMatrix);
		void bindMaterial(const Camera& camera, const int materialIndex);

	};
} 

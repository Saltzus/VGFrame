#pragma once

#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <vector>

#include "Material.h"
#include "Renderer.h"

#include "UniformBuffers/MatrixBuffer.h"
#include "UniformBuffers/LightBuffer.h"
#include "UniformBuffers/PBRBuffer.h"

namespace VGF
{
	struct Mesh
	{	
		glm::mat4 matrix;

		std::vector <float> vertices;
		std::vector <unsigned int> indices;
	};

	class Model
	{
	public:
		Model(const std::string modelPath, const PipelineConfig& config, const std::vector<UniformBufferObject*> additionalUniformBuffers = {});
		Model(std::vector <float> vertices, std::vector <unsigned int> indices, Material* material, const PipelineConfig& config, const std::vector<UniformBufferObject*> additionalUniformBuffers = {});

		Model(const std::string modelPath, const std::vector<UniformBufferObject*> additionalUniformBuffers = {})
			: Model(modelPath, VGF::PipelineConfig::GetDefault(), additionalUniformBuffers) {}

		Model(std::vector <float> vertices, std::vector <unsigned int> indices, Material* material, const std::vector<UniformBufferObject*> additionalUniformBuffers = {})
			: Model(vertices, indices, material, VGF::PipelineConfig::GetDefault(), additionalUniformBuffers) {
		}
		Model(std::vector <float> vertices, std::vector <unsigned int> indices, const std::vector<UniformBufferObject*> additionalUniformBuffers = {})
			: Model(vertices, indices, VGF::Material::getDefaultMaterial(), additionalUniformBuffers) {}

		~Model();

		PipelineConfig config;

		void Render(const glm::mat4& parentMatrix, const Camera& camera, const std::vector<UniformBufferObject*> additionalUniformBuffers = {}) { Render(parentMatrix, camera, config, additionalUniformBuffers); }
		void Render(const glm::mat4& parentMatrix, const Camera& camera, const PipelineConfig& config, const std::vector<UniformBufferObject*> additionalUniformBuffers = {});
		
		void BatchRender(const glm::mat4& parentMatrix, const Camera& camera, const std::vector<UniformBufferObject*> oneTimeAdditionalUniformBuffers = {}, const std::vector<UniformBufferObject*> instanceAdditionalUniformBuffers = {}) { BatchRender(parentMatrix, camera, config, oneTimeAdditionalUniformBuffers, instanceAdditionalUniformBuffers); }
		void BatchRender(const glm::mat4& parentMatrix, const Camera& camera, const PipelineConfig& config, const std::vector<UniformBufferObject*> oneTimeAdditionalUniformBuffers = {}, const std::vector<UniformBufferObject*> instanceAdditionalUniformBuffers = {});

		tinygltf::Model model;
		std::vector <Mesh> meshes;

		bool isCustomModel() { return customModel; }

	private:
		void Draw(const glm::mat4& parentMatrix, const Renderer* renderer, const int bindMaterial, const PipelineConfig& config, const Camera& camera, const std::vector<UniformBufferObject*> additionalUniformBuffers = {});
		void BatchDraw(const glm::mat4& parentMatrix, const Renderer* renderer,const int bindMaterial, const PipelineConfig& config, const Camera& camera, const std::vector<UniformBufferObject*> oneTimeAdditionalUniformBuffers = {}, const std::vector<UniformBufferObject*> instanceAdditionalUniformBuffers = {});
		void drawNodes(int nodeIdx, const glm::mat4& parentMatrix, const PipelineConfig& config, const Camera& camera, LightBufferObject lightBuffer);

		std::vector<UniformBufferObject*> _additionalUniformBuffers;
		std::vector<UniformBufferObject*> uniformBuffers =
		{
			new MatrixBufferObject,
			new PBRbufferObject,
			new LightBufferObject
		};

		enum uniformBufferEnum
		{
			matrixBuffer,
			pbrBuffer,
			lightBuffer
		};

		std::vector<Renderer*> modelRenderers;
		Material* _material = nullptr;

		bool customModel = false;

		struct VaoRange
		{
			int begin; // Index of first element in vertexArrayObjects
			int count; // Number of elements in range
		};

		std::vector <Texture*> textures;

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

		tinygltf::TinyGLTF loader;
		std::string err;
		std::string warn;

		std::vector<unsigned int> vertexArrayObjects;
		std::vector<VaoRange> meshToVertexArrays;


		void LoadMeshData
		(
			const tinygltf::Model& model,
			tinygltf::Mesh& mesh,
			std::vector<float>& outVertices,
			std::vector<uint32_t>& outIndices
		);

		std::vector<Texture*> createTextureObjects(const tinygltf::Model& model) const;
		glm::mat4 getLocalToWorldMatrix(const tinygltf::Node& node, const glm::mat4& parentMatrix);
		UniformBufferObject* bindMaterial(const Camera& camera, const int materialIndex);
	};
} 

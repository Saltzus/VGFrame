#pragma once

#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <vector>

#include "Material.h"
#include "Renderer.h"

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
		Model(const char* modelPath);
		Model(std::vector <float> vertices, std::vector <unsigned int> indices, Material* material);
		Model(Mesh& mesh, Material* material);

		~Model();

		void Model::renderModel(std::vector<Renderer*> renderers, const glm::mat4& parentMatrix, PipelineConfig& config, Camera* camera);
		tinygltf::Model model;
		std::vector <Mesh> meshes;

		bool isCustomModel() { return customModel; }

	private:
		void Model::drawNodes(std::vector<Renderer*> renderers, int nodeIdx, const glm::mat4& parentMatrix, PipelineConfig& config, Camera* camera);

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
		const PBRbufferObject bindMaterial(Camera* camera, const int materialIndex);
	};
} 

#pragma once

#include <tiny_gltf.h>
#include <glm/glm.hpp>
#include <vector>

#include "Texture.h"
#include "Renderer.h"

namespace VGF
{
	struct Mesh
	{	
		void createRenderer()
		{
			this->renderer = new Renderer(indices, vertices);
		}

		Renderer* renderer;

		glm::mat4 matrix;

		std::vector <float> vertices;
		std::vector <unsigned int> indices;
	};

	class Model
	{
	public:
		Model(const char* modelPath);
		~Model();

		void Model::drawNodes(int nodeIdx, const glm::mat4& parentMatrix, PipelineConfig& config, Camera* camera);
		tinygltf::Model model;

	private:

		struct VaoRange
		{
			int begin; // Index of first element in vertexArrayObjects
			int count; // Number of elements in range
		};

		std::vector <Texture*> textures;
		std::vector <Mesh> meshes;

		unsigned int uLightDirectionLocation;
		unsigned int uLightIntensity;

		unsigned int uBaseColorTexture;
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

		std::vector<unsigned int> createBufferObjects(const tinygltf::Model& model);

		std::vector<unsigned int> createVertexArrayObjects(const tinygltf::Model& model,
			const std::vector<unsigned int>& bufferObjects,
			std::vector<VaoRange>& meshToVertexArrays);

		std::vector<Texture*> createTextureObjects(const tinygltf::Model& model) const;

		const void bindMaterial(const int materialIndex);

		glm::mat4 getLocalToWorldMatrix(const tinygltf::Node& node, const glm::mat4& parentMatrix);
	};
} 

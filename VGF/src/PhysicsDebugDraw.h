#pragma once

#include <btBulletCollisionCommon.h>
#include <glad/glad.h>
#include <glm/matrix.hpp>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Object.h"
#include "PipelineConfig.h"



class PhysicsDebugDraw : public btIDebugDraw 
{
public:

	std::vector<float> vertices;
	std::vector<unsigned int>indices;

	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		// Vertex 1
		vertices.push_back(from.x());	// position 1
		vertices.push_back(from.y());
		vertices.push_back(from.z());

		vertices.push_back(0);			// normal 1
		vertices.push_back(0);
		vertices.push_back(0);

		vertices.push_back(color.x());	// color 1
		vertices.push_back(color.y());
		vertices.push_back(color.z());

		vertices.push_back(0.0f);		// textureCoordinates 1
		vertices.push_back(0.0f);

		// Vertex 2
		vertices.push_back(to.x());		// position 2
		vertices.push_back(to.y());
		vertices.push_back(to.z());

		vertices.push_back(0);			// normal 2
		vertices.push_back(0);
		vertices.push_back(0);

		vertices.push_back(color.x());	// color 2
		vertices.push_back(color.y());
		vertices.push_back(color.z());

		vertices.push_back(0.0f);		// textureCoordinates 2
		vertices.push_back(0.0f);

		// Indices for the vertex pair
		indices.push_back(static_cast<GLuint>(indices.size()));
		indices.push_back(static_cast<GLuint>(indices.size()));
	}
	virtual void drawContactPoint(const btVector3&, const btVector3&, btScalar, int, const btVector3&) {}
	virtual void reportErrorWarning(const char*) {}
	virtual void draw3dText(const btVector3&, const char*) {}
	virtual void setDebugMode(int p) {
		m = p;
	}
	int getDebugMode(void) const { return 3; }
	int m;
};
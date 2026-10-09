#include "PhysicsDebugRenderer.h"

#include <glm/gtc/quaternion.hpp>

#include "Physics.h"
#include <VGF/DebugShapes.h>

PhysicsDebugRenderer::PhysicsDebugRenderer()
{
}

constexpr glm::vec3 HexToRGB(uint32_t hex)
{
    return
    {
        ((hex >> 16) & 0xFF) / 255.0f,
        ((hex >> 8) & 0xFF) / 255.0f,
        (hex & 0xFF) / 255.0f
    };
}

void DebugDrawSegment(b3Pos p1, b3Pos p2, b3HexColor color, void* context)
{
    auto* self = static_cast<PhysicsDebugRenderer*>(context);
    glm::vec3 col = HexToRGB(color);

    self->wireframeVertices.insert(self->wireframeVertices.end(),
        {
            float(p1.x), float(p1.y), float(p1.z), col.r, col.g, col.b,
            float(p2.x), float(p2.y), float(p2.z), col.r, col.g, col.b
        });

    unsigned int base = static_cast<unsigned int>(self->wireframeVertices.size()) / 6;
    self->wireframeIndices.push_back(base);
    self->wireframeIndices.push_back(base + 1);
}

void DebugDrawMeshData(MeshData data , b3HexColor color, void* context)
{
    auto* self = static_cast<PhysicsDebugRenderer*>(context);
    glm::vec3 col = HexToRGB(color);

    unsigned int base = static_cast<unsigned int>(self->wireframeVertices.size()) / 6;

    for (size_t i = 0; i < data.vertices.size(); i+=3)
    {
        self->wireframeVertices.insert(self->wireframeVertices.end(), { data.vertices[i], data.vertices[i + 1], data.vertices[i + 2], col.x, col.y, col.z });
    }
    
    self->wireframeIndices.reserve(self->wireframeIndices.size() + data.indices.size());
    for (unsigned int index : data.indices)
    {
        self->wireframeIndices.push_back(base + index);
    }
}

bool DebugDrawShape(void* userShape, b3WorldTransform transform, b3HexColor color, void* context)
{
    if (!userShape) return false;

    DebugShapes::Shape* shape = static_cast<DebugShapes::Shape*>(userShape);
    if (shape->data.vertices.empty()) return true;

    std::vector<float> transformedVertices;
    transformedVertices.reserve(shape->data.vertices.size());

    glm::vec3 position(transform.p.x, transform.p.y, transform.p.z);
    glm::quat rotation(transform.q.s, transform.q.v.x, transform.q.v.y, transform.q.v.z);

    for (size_t i = 0; i < shape->data.vertices.size(); i+=3)
    {
        glm::vec3 localVert(shape->data.vertices[i], shape->data.vertices[i + 1], shape->data.vertices[i + 2]);
        glm::vec3 worldVert = (rotation * localVert) + position;
        transformedVertices.insert(transformedVertices.end(), { worldVert.x, worldVert.y, worldVert.z });
    }

    DebugDrawMeshData({transformedVertices, shape->data.indices}, color, context);
    return true;
}

void DebugDrawBox(b3Vec3 extents, b3WorldTransform transform, b3HexColor color, void* context)
{
}

void PhysicsDebugRenderer::Debug(b3WorldId world)
{
    b3DebugDraw debugDraw = b3DefaultDebugDraw();
    debugDraw.context = this;
    debugDraw.DrawSegmentFcn = DebugDrawSegment;
    debugDraw.DrawShapeFcn = DebugDrawShape;
    debugDraw.DrawBoxFcn = DebugDrawBox;

    debugDraw.drawShapes = true;
    debugDraw.drawJoints = true;
    debugDraw.drawBounds = true;
    debugDraw.drawContacts = true;
    debugDraw.drawContactNormals = true;
    debugDraw.drawAnchorA = true;
    debugDraw.drawContactFeatures = true;
    debugDraw.drawMass = true;
    debugDraw.drawContactForces = true;
    debugDraw.drawIslands = true;

    b3World_Draw(world, &debugDraw, UINT64_MAX);
}
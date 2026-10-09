#include "DebugShapes.h"

#include "glm/glm.hpp"

glm::vec3 GetAnyPerpendicularUnitVector(const glm::vec3& vec)
{
    if (vec.y != 0.0f || vec.z != 0.0f) return glm::vec3(1, 0, 0);
    else return glm::vec3(0, 1, 0);
}

std::vector<float> Capsule(b3Vec3 strt, b3Vec3 ed, float radius)
{
    glm::vec3 capsuleStart = { strt.x, strt.y, strt.z };
    glm::vec3 capsuleEnd = { ed.x, ed.y, ed.z };

    std::vector<float> returnVertices;

    const glm::vec3 axis = capsuleEnd - capsuleStart;
    const float     length = glm::length(axis);
    const glm::vec3 localZ = axis / length;
    const glm::vec3 localX = GetAnyPerpendicularUnitVector(localZ);
    const glm::vec3 localY = glm::cross(localZ, localX);

    using glm::cos;
    using glm::sin;
    constexpr float pi = B3_PI;

    const glm::vec3 start(0.0f);
    const glm::vec3 end(1.0f);
    const float     resolution = 16.0f;

    const glm::vec3 step = (end - start) / resolution;

    auto cylinder = [localX, localY, localZ, capsuleStart, radius, length](const float u, const float v)
    {
        return capsuleStart                                  //
            + localX * cos(2.0f * pi * u) * radius //
            + localY * sin(2.0f * pi * u) * radius //
            + localZ * v * length;                   //

    };

    auto sphereStart = [localX, localY, localZ, capsuleStart, radius](const float u, const float v) -> glm::vec3
    {
        const float latitude = (pi / 2.0f) * (v - 1);

        return capsuleStart                                                  //
            + localX * cos(2.0f * pi * u) * cos(latitude) * radius //
            + localY * sin(2.0f * pi * u) * cos(latitude) * radius //
            + localZ * sin(latitude) * radius;
    };

    auto sphereEnd = [localX, localY, localZ, capsuleEnd, radius](const float u, const float v)
    {
        const float latitude = (pi / 2.0f) * v;
        return capsuleEnd                                                    //
            + localX * cos(2.0f * pi * u) * cos(latitude) * radius //
            + localY * sin(2.0f * pi * u) * cos(latitude) * radius //
            + localZ * sin(latitude) * radius;
    };

    for (float i = 0; i < resolution; ++i)
    {
        for (float j = 0; j < resolution; ++j)
        {
            const float u = i * step.x + start.x;
            const float v = j * step.y + start.y;

            const float un = (i + 1 == resolution) ? end.x : (i + 1) * step.x + start.x;
            const float vn = (j + 1 == resolution) ? end.y : (j + 1) * step.y + start.y;

            // Draw Cylinder
            {
                const glm::vec3 p0 = cylinder(u, v);
                const glm::vec3 p1 = cylinder(u, vn);
                const glm::vec3 p2 = cylinder(un, v);
                const glm::vec3 p3 = cylinder(un, vn);

                returnVertices.insert
                (
                    returnVertices.end(),
                    { 
                        p0.x, p0.y, p0.z,
                        p1.x, p1.y, p1.z,
                        p2.x, p2.y, p2.z,
                        p3.x, p3.y, p3.z,
                    }
                );
            }

            // Draw Sphere start
            {
                const glm::vec3 p0 = sphereStart(u, v);
                const glm::vec3 p1 = sphereStart(u, vn);
                const glm::vec3 p2 = sphereStart(un, v);
                const glm::vec3 p3 = sphereStart(un, vn);

                returnVertices.insert
                (
                    returnVertices.end(),
                    {
                        p0.x, p0.y, p0.z,
                        p1.x, p1.y, p1.z,
                        p2.x, p2.y, p2.z,
                        p3.x, p3.y, p3.z,
                    }
                );
            }

            // Draw Sphere end
            {
                const glm::vec3 p0 = sphereEnd(u, v);
                const glm::vec3 p1 = sphereEnd(u, vn);
                const glm::vec3 p2 = sphereEnd(un, v);
                const glm::vec3 p3 = sphereEnd(un, vn);

                returnVertices.insert
                (
                    returnVertices.end(),
                    {
                        p0.x, p0.y, p0.z,
                        p1.x, p1.y, p1.z,
                        p2.x, p2.y, p2.z,
                        p3.x, p3.y, p3.z,
                    }
                );
            }
        }
    }

    return returnVertices;
}
MeshData Hull(const b3HullData* hull)
{
    if (hull != nullptr && hull->vertexCount > 0)
    {
        MeshData data;

        const b3Vec3* points = b3GetHullPoints(hull);
        if (points == nullptr) return {};
        data.vertices.resize(hull->vertexCount * 3);
        std::memcpy(data.vertices.data(), points, hull->vertexCount * sizeof(b3Vec3));

        const b3HullHalfEdge* edges = b3GetHullEdges(hull);
        if (edges == nullptr) return {};
        data.indices.reserve(hull->edgeCount);
        
        for (size_t i = 0; i < hull->edgeCount; i++)
        {
            int origin = edges[i].origin;
            int destination = edges[edges[i].twin].origin;

            if (origin < destination)
            {
                data.indices.push_back(static_cast<unsigned int>(origin));
                data.indices.push_back(static_cast<unsigned int>(destination));
            }
        }
        return data;
    }
    return {};
}

DebugShapes::Shape::Shape(const b3DebugShape* b3shape)
{
    switch (b3shape->type)
    {
        case b3_capsuleShape:
        {
            //vertices = Capsule(b3shape->capsule->center1, b3shape->capsule->center2, b3shape->capsule->radius);
            break;
        }
        case b3_hullShape:
        {
            data = Hull(b3shape->hull);
        }
        default:
        {
            break;
        }
    }
}
#pragma once

#include "Renderer.h"
#include "Material.h"

#include "Model.h"

namespace VGF
{
    class Object
    {
    public:
        Object(Model* model) : _model(model) {}
        ~Object() = default;

        std::vector<UniformBufferObject*> additionalUniformBuffers;

        virtual void Render(const Camera& camera, const std::vector<UniformBufferObject*> additionalUniformBuffers = {});
        void Render(const Camera& camera, const PipelineConfig& config, const std::vector<UniformBufferObject*> additionalUniformBuffers = {});
        void SetModel(Model* model) { _model = model; }
        void SetConfig(const PipelineConfig& config) { _model->config = config; }

        virtual void SetPosition(const float x, const float y, const float z) { _position = glm::vec3(x, y, z); }
        void SetPosition(const glm::vec3 position) { SetPosition(position.x, position.y, position.z); }

        virtual void SetRotation(const glm::quat rotation) { _rotation = rotation; }
        void SetRotation(const float x, const float y, const float z) { SetRotation(glm::quat(glm::radians(glm::vec3(x, y, z))));}
        void SetRotation(const glm::vec3 rotation) { SetRotation(rotation.x, rotation.y, rotation.z); }

        virtual void SetScale(const float x, const float y, const float z) { _scale = glm::vec3(x, y, z); }
        void SetScale(const glm::vec3 scale) { SetScale(scale.x, scale.y, scale.z); }

        void LookAt(const glm::vec3 lookingPosition, const float xOffset = 0.f, const float yOffset = 0.f);

    protected:
        Model* _model;

        glm::vec3 _position = glm::vec3(0.f, 0.f, 0.f);
        glm::quat _rotation = glm::quat(0.f, 0.f, 0.f, 1.f);
        glm::vec3 _scale = glm::vec3(0.7f, 0.7f, 0.7f);

        virtual glm::mat4 CreateModelMatrix() const;
    private:
    };
    
} 

#include "Object.h"
#include <glm/gtx/matrix_decompose.hpp>

namespace VGF
{
    void Object::Render(const Camera& camera, const std::vector<UniformBufferObject*> additionalUniformBuffers)
    {
        if (!additionalUniformBuffers.empty()) {
            this->additionalUniformBuffers = additionalUniformBuffers;
        }

        glm::mat4 model = CreateModelMatrix();
        _model->Render(model, camera, this->additionalUniformBuffers);
    }

    void Object::Render(const PipelineConfig& config, const Camera& camera, const std::vector<UniformBufferObject*> additionalUniformBuffers)
    {
        _model->config = config;
        Render(camera, additionalUniformBuffers);
    }

    void Object::LookAt(glm::vec3 lookingPosition, float xOffset, float yOffset)
    {
        glm::vec3 direction = glm::normalize(lookingPosition - _position);
        float yaw = atan2(direction.x, direction.z);
        float pitch = -asin(glm::clamp(direction.y, -1.f, 1.f));

        glm::quat qYaw = glm::angleAxis(yaw, glm::vec3(0, 1, 0));
        glm::quat qPitch = glm::angleAxis(pitch, glm::vec3(1, 0, 0));

        glm::quat qXOff = glm::angleAxis(glm::radians(xOffset), glm::vec3(0, 1, 0));
        glm::quat qYOff = glm::angleAxis(glm::radians(yOffset), glm::vec3(0, 1, 0));

        glm::quat base = qYaw * qPitch;
        glm::quat offset = qXOff * qYOff;
        SetRotation(offset * base);
    }

    glm::mat4 Object::CreateModelMatrix() const
    {
        glm::mat4 model = glm::mat4(1.f);

        model = glm::translate(model, _position);
        model *= glm::mat4_cast(_rotation);
        model = glm::scale(model, _scale);

        return model;
    }

}
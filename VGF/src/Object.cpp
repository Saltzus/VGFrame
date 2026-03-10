#include "Object.h"

namespace VGF
{
    

    Object::Object(Model* model)
    {
        this->model = model;
    }

    void Object::SetModel(Model* model)
    {
        this->model = model;
    }
    
    Object::~Object()
    {
    }

    void Object::Render(PipelineConfig& config, Camera* camera, std::vector<UniformBufferObject*> additionalUniformBuffers)
    {
        glm::mat4 model = glm::mat4(1.f);

        model = glm::translate(model, translation);

        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, scale);

        this->model->renderModel(model, config, camera, additionalUniformBuffers);
    }
}
#include "Object.h"

namespace VGF
{
    

    Object::Object(Model* model)
    {
        this->model = model;

        for (auto mesh : this->model->meshes)
            modelRenderers.emplace_back(new Renderer(mesh.indices, mesh.vertices));
    }

    void Object::SetModel(Model* model)
    {
        for (auto renderer : modelRenderers)
            delete renderer;

        modelRenderers.clear();

        this->model = model;

        for (auto mesh : this->model->meshes)
            modelRenderers.emplace_back(new Renderer(mesh.indices, mesh.vertices));
    }
    
    Object::~Object()
    {
        for (auto renderer : modelRenderers)
            delete renderer;
    }

    void Object::Render(PipelineConfig& config, Camera* camera, glm::mat4 model)
    {
        model = glm::translate(model, translation);

        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, scale);

        this->model->renderModel(modelRenderers, model, config, camera);
    }
}
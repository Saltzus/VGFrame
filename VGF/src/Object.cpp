#include "Object.h"

namespace VGF
{
    

    Object::Object()
    {
        texture = Texture::GetDefaultTexture();
        //renderer = new Renderer(indices, vertices);
        
        model = new Model("../../../Examples/HelloWorld/Models/Duck.gltf");
    }

    void Object::SetModel(Model* model)
    {
        this->model = model;
    }
    
    Object::~Object()
    {
        //delete this->renderer;
        delete this->texture;

        delete model;
    }

    void Object::Render(PipelineConfig& config, Camera* camera, glm::mat4 model)
    {
        config.Activate();

        texture->Bind();

        model = glm::translate(model, translation);

        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        model = glm::scale(model, scale);

        

        for (const auto nodeIdx : this->model->model.scenes[this->model->model.defaultScene].nodes)
            this->model->drawNodes(0, model, config, camera);

        //this->renderer->Render(config, camera, model);
    }
}
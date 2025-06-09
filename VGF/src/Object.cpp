#include "Object.h"

namespace VGF
{
    

    Object::Object()
    {
        texture = Texture::GetDefaultTexture();
        this->renderer = new Renderer(indices, vertices);
    }
    
    Object::~Object()
    {
        delete this->renderer;
        delete this->texture;
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

        this->renderer->Render(config, camera, model);
    }
}
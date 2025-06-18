#pragma once

#include "Renderer.h"
#include "Texture.h"

#include "Model.h"

namespace VGF
{
    class Object
    {
    public:
        Object();
        ~Object();

        virtual void Render(PipelineConfig& config, Camera* camera, glm::mat4 model = glm::mat4(1.0f));
        void SetModel(Model* model);

		Texture* texture;  


        Model* model;

	    glm::vec3 translation = glm::vec3(0.f, 0.f, 0.f);
	    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	    glm::vec3 scale = glm::vec3(0.7f, 0.7f, 0.7f);

    protected:
        Renderer* renderer;

    private:
       

    };
    
} 

#include <iostream>
#include <Core.h>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char** argv)
{
    VGF::Window window("VGF - Test", SCR_WIDTH, SCR_HEIGHT);
    VGF::Renderer::InitApi(window); // TODO : add automatically run compile.bat and add switch to change from opengl to vulkan

    VGF::Shader defaultShader("../../../Examples/HelloWorld/Shaders/default.vert", "../../../Examples/HelloWorld/Shaders/default.frag");
    //VGF::Shader reversedShader("../../../Examples/HelloWorld/Shaders/halftransparent.vert", "../../../Examples/HelloWorld/Shaders/halftransparent.frag");
    VGF::Texture* texture = new VGF::Texture("../../../Examples/HelloWorld/Textures/PixelText.png");


    VGF::Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.f, 0.f, -8.f)); // TODO : add camera shader

    VGF::Sprite sprite(window);
    VGF::Sprite sprite1(window);



    VGF::Audio audio;
    audio.PlayAudio("../../../Examples/HelloWorld/Audio/BIG-Trouble.ogg");

    while (!glfwWindowShouldClose(window))
    {
        static auto startTime = std::chrono::high_resolution_clock::now();

        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        camera.updateMatrix(45.0f, 0.01f, 10000.0f, true);


        sprite.scale = { glm::sin(time) + 2, glm::cos(time), -1 };
        sprite.rotation.y = time * 180.0f;
        sprite.rotation.x = 45;
        sprite.translation = { glm::sin(time) + 2, glm::cos(time), -1 };
        sprite.texture = texture;

        sprite.Render(&defaultShader, &camera);

        sprite1.scale = { glm::sin(time) + 2, glm::cos(time), -1 };
        sprite1.rotation.y = time * 180.0f;
        sprite1.rotation.x = 45;
        sprite1.translation = { glm::sin(-time) - 2, glm::cos(-time), -1 };
        //sprite1.texture = texture;

        sprite1.Render(&defaultShader, &camera);



        VGF::Input::processInput(window);

        VGF::Renderer::RenderGraphics();
        window.Display();
    }

    glfwTerminate();
    return 0;
}
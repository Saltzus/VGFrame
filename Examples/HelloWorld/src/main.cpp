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


    VGF::Physics physics;
    VGF::Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.f, 3.f, -40.f)); // TODO : add camera shader

    VGF::PhysicsObject groundObject(window, &physics, {0,-56,0}, {50,50,50}, 0.f);
    
    std::vector<VGF::PhysicsObject*> objects;

    for (size_t i = 0; i < 25; i++)
    {
        objects.push_back(new VGF::PhysicsObject(window, &physics, { (btScalar)glm::sin(i), 120, (btScalar)glm::cos(i)}, {2,2,2}, 1.f));
    }

    btAlignedObjectArray<btRigidBody*> bodies = physics.dynamicsWorld->getNonStaticRigidBodies();

    VGF::Audio audio;
    audio.PlayAudio("../../../Examples/HelloWorld/Audio/BIG-Trouble.ogg");

    double delta_time = 0.0f;
    double last_frame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        double current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;

        camera.updateMatrix(45.0f, 0.01f, 10000.0f);
        camera.Inputs(window, delta_time);


        groundObject.Render(&defaultShader, &camera);

        for (auto object : objects)
            object->Render(&defaultShader, &camera);

        physics.Update(delta_time);

        VGF::Input::processInput(window);

        VGF::Renderer::RenderGraphics();
        window.Display();
    }

    for (auto object : objects)
        delete object;

    glfwTerminate();
    return 0;
}
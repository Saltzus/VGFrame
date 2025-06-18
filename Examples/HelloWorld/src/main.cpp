#include <iostream>
#include <Core.h>

#include <tiny_gltf.h>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char** argv)
{
    VGF::Window window("VGF - Test", SCR_WIDTH, SCR_HEIGHT);
    VGF::Renderer::InitApi(window); // TODO : add automatically run compile.bat and add switch to change from opengl to vulkan

    VGF::PipelineConfig debugPipeline
    (
        "../../../Examples/HelloWorld/Shaders/debug.vert",
        "../../../Examples/HelloWorld/Shaders/debug.frag",
        VGF::Topology::TRIANGLE_LIST
    );

    VGF::PipelineConfig defaultPipeline
    (
        "../../../Examples/HelloWorld/Shaders/default.vert",
        "../../../Examples/HelloWorld/Shaders/default.frag",
        VGF::Topology::TRIANGLE_LIST
    );

    VGF::Texture* texture = new VGF::Texture("../../../Examples/HelloWorld/Textures/PixelText.png");


    VGF::Model cubeModel("../../../Examples/HelloWorld/Models/Cube.gltf");

    VGF::Physics physics;
    VGF::Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.f, 3.f, -40.f)); // TODO : add camera shader

    VGF::PhysicsObject groundObject(&physics, { 0,-56,0 }, { 50,50,50 }, 0.f);
    VGF::PhysicsObject physicsObject(&physics, { -0.5,5,0 }, { 2,2,2 }, 1.f);
    VGF::PhysicsObject physicsObject1(&physics, {0.5,5,0}, {2,2,2}, 1.f);

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

        //debugShader.Activate();
        //texture->Bind();
        groundObject.Render(debugPipeline, &camera);
        groundObject.SetModel(&cubeModel);
        //defaultShader.Activate();
        physicsObject.Render(defaultPipeline, &camera);
        physicsObject1.Render(defaultPipeline, &camera);

        VGF::PhysicsObject* object = VGF::Input::pickObject(&window, physics.dynamicsWorld, &camera);
        if (object)
        {
            object->body->activate(true);

            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
                object->body->applyCentralImpulse(btVector3(0, 0.1, 0));

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
                object->body->setAngularVelocity(btVector3(0,-5,0));

            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
                object->body->setAngularVelocity(btVector3(0, 5, 0));
        }

        physics.Update(delta_time);

        VGF::Input::processInput(window);

        physics.debugRender(&window, &camera);
        VGF::Renderer::RenderGraphics();
        window.Display();
    }

    defaultPipeline.Delete();

    glfwTerminate();
    return 0;
}
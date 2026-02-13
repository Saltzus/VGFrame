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

    VGF::PipelineConfig depthPipeline
    (
        "../../../Examples/HelloWorld/Shaders/depth.vert",
        "../../../Examples/HelloWorld/Shaders/depth.frag",
        VGF::Topology::TRIANGLE_LIST
    );

    VGF::Texture* texture = new VGF::Texture("../../../Examples/HelloWorld/Textures/PixelText.png"); texture->Bind(VGF::textureType::color);
    VGF::Texture* texture1 = new VGF::Texture("../../../Examples/HelloWorld/Textures/Image_0.png");  texture1->Bind(VGF::textureType::metallicRoughness);
    VGF::Texture* texture2 = new VGF::Texture("../../../Examples/HelloWorld/Textures/Image_0.png");  texture2->Bind(VGF::textureType::emissive);
    VGF::Texture* texture3 = new VGF::Texture("../../../Examples/HelloWorld/Textures/Image_0.png");  texture3->Bind(VGF::textureType::occulsion);
    VGF::Texture* texture4 = new VGF::Texture("../../../Examples/HelloWorld/Textures/Image_0.png");  texture4->Bind(VGF::textureType::normal);

    VGF::Model cubeModel("../../../Examples/HelloWorld/Models/Cube.gltf");
    VGF::Model planeModel("../../../Examples/HelloWorld/Models/Plane.gltf");

    VGF::Model duckModel("../../../Examples/HelloWorld/Models/Duck.gltf");

    const unsigned char data[4 * 125 * 125] = { 120 };
    VGF::Texture text(data, 0, 125,125);
    VGF::FrameBuffer framebuffer(text);

    std::vector<float> vertices =
    {
        0, 0.5, -0.1,     0.0f, 0.0995f, 0.9950f,    1,1,1, 0.5,0,
        0.5, -0.5 ,0,     0.0f, 0.0995f, 0.9950f,    1,1,1, 1,1,
        -0.5, -0.5, 0,    0.0f, 0.0995f, 0.9950f,    1,1,1, 0,1
    };

    std::vector<unsigned int> indices =
    {
        0,1,2
    };

    VGF::Material mat = *VGF::Material::getDefaultMaterial();
    mat.color = &text;

    VGF::Model triangle(vertices, indices, &mat);

    VGF::Physics physics;
    VGF::Camera camera(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0.f, 29.f, -10.f));
    VGF::Camera camera2(SCR_WIDTH, SCR_HEIGHT, glm::vec3(0, 20, 0));
    camera2.Orientation = glm::rotate(camera2.Orientation, glm::radians((float) - 90), glm::normalize(glm::cross(camera2.Orientation, camera2.Up)));

    VGF::PhysicsObject physicsObject(&duckModel, &physics, { 0.5,5,0 }, { 2,2,2 }, 1.f);

    VGF::PhysicsObject physicsObject1(&triangle, &physics, {0.5,5,0}, {2,2,2}, 1.f);

    VGF::PhysicsObject groundObject(&cubeModel, &physics, { 0,-56,0 }, { 50,50,50 }, 0.f);
    
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
        camera2.updateMatrix(45.0f, 0.01f, 100.0f);


        framebuffer.Bind();
        //physicsObject1.Render(defaultPipeline, &camera2);
        //physicsObject.Render(depthPipeline, &camera2);
        //groundObject.Render(depthPipeline, &camera2);
        framebuffer.UnBind();

        physicsObject.Render(defaultPipeline, &camera);
        groundObject.Render(defaultPipeline, &camera);
        physicsObject1.Render(defaultPipeline, &camera);

        VGF::PhysicsObject* object = VGF::Input::pickObject(&window, physics.dynamicsWorld, &camera);
        if (object)
        {
            object->body->activate(true);

            if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            {
                btVector3 impulse = btVector3(0, 50, 0) * delta_time;
                object->body->applyCentralImpulse(impulse);
            }

            if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            {
                btVector3 angVel = object->body->getAngularVelocity();
                angVel += btVector3(0, -50, 0) * delta_time;
                object->body->setAngularVelocity(angVel);
            }

            if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            {
                btVector3 angVel = object->body->getAngularVelocity();
                angVel += btVector3(0, 50, 0) * delta_time;
                object->body->setAngularVelocity(angVel);
            }
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
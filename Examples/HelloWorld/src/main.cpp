#include <iostream>
#include <VGF/VGF.h>

#include <tiny_gltf.h>

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main(int argc, char** argv)
{
    // Creating window and adding resourcepaths for project

    VGF::Window window("VGF - HelloWorld", SCR_WIDTH, SCR_HEIGHT);

    VGF::Resource::AddResourcePath("Examples/HelloWorld/");

    // Creating a shader pipeline

    VGF::PipelineConfig depthPipeline
    (
        VGF::Resource::Get("Shaders/depth_vert.slang"),
        VGF::Resource::Get("Shaders/depth_frag.slang"),
        VGF::Topology::TRIANGLE_LIST
    );

    // Loading texture and binding to color (binding usually done trough materials)

    VGF::Texture* texture =  new VGF::Texture(VGF::Resource::Get("Textures/PixelText.png")); 
    texture->Bind(VGF::textureType::color);


    // Create models from file

    VGF::Model cubeModel(VGF::Resource::Get("Models/Cube.gltf"));
    VGF::Model planeModel(VGF::Resource::Get("Models/Plane.gltf"));

    VGF::Model duckModel(VGF::Resource::Get("Models/Duck.gltf"));
    VGF::Model duckModel2(VGF::Resource::Get("Models/Duck.gltf"));

    std::vector<float> vertices =
    {
//      Position          Normal                     VertexColor    UV      JointIndices    JointWeights

        0, 0.5, -0.1,     0.0f, 0.0995f, 0.9950f,    1,0,1,         0.5,0,  0,0,0,0,        0,0,0,0,
        0.5, -0.5 ,0,     0.0f, 0.0995f, 0.9950f,    1,1,0,         1,1,    0,0,0,0,        0,0,0,0,
        -0.5, -0.5, 0,    0.0f, 0.0995f, 0.9950f,    0,1,1,         0,1,    0,0,0,0,        0,0,0,0
    };

    std::vector<unsigned int> indices =
    {
        0,1,2
    };

    // Creating a texture where framebuffer renders

    const unsigned char data[4 * 125 * 125] = { 120 };
    VGF::Texture framebufferTexture(data, 0, 125, 125);

    VGF::FrameBuffer framebuffer(framebufferTexture);

    // Using the texture that framebuffer renders and putting it on a triangle

    VGF::Material material = *VGF::Material::getDefaultMaterial();
    material.color = &framebufferTexture;

    // Create model with custom material, vertices and indices

    VGF::Model triangle(vertices, indices, &material);

    VGF::Physics physics;
    VGF::Camera camera(glm::vec3(0.f, 29.f, -10.f));
    VGF::Camera camera2(glm::vec3(0.f, 0.f, -10.f));

    // You can create physics or normal objects

    VGF::PhysicsObject physicsObject(duckModel, &physics, { 0.5,5,0 }, { 2,2,2 });
    VGF::PhysicsObject physicsObject1(triangle, &physics, {-0.5,5,0}, {2,2,2});

    VGF::PhysicsObject groundObject(cubeModel, &physics, { 0,-56,0 }, { 50,50,50 }, b3_staticBody);

    VGF::Object duck(duckModel2);

    // Audio not implemented fully

    VGF::Audio audio;
    //audio.PlayAudio(VGF::Resource::Get("Audio/"));

    float time = 0;
    while (!glfwWindowShouldClose(window))
    {
        VGF::Time::UpdateDeltatime();

        camera.updateMatrix(window,45.0f, 0.01f, 10000.0f);
        camera.Inputs(window, VGF::Time::GetDeltaTime());
        camera2.updateMatrix(window, 45.0f, 0.01f, 100.0f);

        time += VGF::Time::GetDeltaTime();
        camera2.position.z = 10 - std::sin(time) * 7;

        framebuffer.Bind();
        // Needs to have different VGF::Model to be able to use different camera (atleast for now)
        duck.Render(camera2, depthPipeline);
        framebuffer.UnBind();

        physicsObject.Render(camera);
        groundObject.Render(camera);
        physicsObject1.Render(camera);


        // Selecting physics bodies from physics

        b3BodyId body;
        glm::vec3 hitPos;

        VGF::RaycastContext context = VGF::Input::pickObject(&window, physics, &camera, body, hitPos, nullptr);
        if (context.hit && VGF::Input::pressingMouseButton(window, GLFW_MOUSE_BUTTON_1))
        {
            b3Body_SetAngularVelocity(context.hitBody, { 0.f,5.f,0.f });
        }


        physics.Update(VGF::Time::GetDeltaTime());

        VGF::Input::processInput(window);

        physics.DebugRender(camera);
        VGF::Renderer::RenderGraphics();
        window.Display();
    }

    return 0;
}
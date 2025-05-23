#include <iostream>
#include <raylib.h>

#include "Server/Server.h"

int main() 
{
    Server server;

    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");

    SetTargetFPS(60); // Miika ei saa poistaa tätä >:(

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {

        // Draw
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        // end draw
    }

    CloseWindow(); // Close window and OpenGL context

    return 0;
}
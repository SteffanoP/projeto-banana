/*******************************************************************************************
*
*   raylib [textures] example - Texture loading and drawing a part defined by a rectangle
*
*   This example has been created using raylib 1.3 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#define MAX_FRAME_SPEED     15
#define MIN_FRAME_SPEED      1

int main(void)
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [texture] example - texture rectangle");

    // NOTE: Textures MUST be loaded after Window initialization (OpenGL context is required)
    Texture2D companheiro = LoadTexture("sprites/companheiro-da-silva.png");        // Texture loading

    Vector2 position = { 350.0f, 280.0f };
    Rectangle frameRec = { 0.0f, 0.0f, (float)companheiro.width/4, (float)companheiro.height/4 };
    int currentFrame = 0;
    int framesCounter = 0;
    int framesSpeed = 8;            // Number of spritesheet frames shown by second

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        framesCounter++;

        if (framesCounter >= (60/framesSpeed))
        {
            framesCounter = 0;
            currentFrame++;
            

            if (currentFrame > 14){
                currentFrame = 0;
                frameRec.y = 0.0f;
            }
            else if (currentFrame > 12)
            {
                frameRec.y = (float)currentFrame*(float)companheiro.height;
            }
            else if (currentFrame > 8)
            {
                frameRec.y = (float)currentFrame*(float)companheiro.height;
            }
            else if (currentFrame > 4)
            {
                frameRec.y = (float)currentFrame*(float)companheiro.height;
            }
            

            frameRec.x = (float)currentFrame*(float)companheiro.width/4;
        }

        if (IsKeyPressed(KEY_RIGHT)) framesSpeed++;
        else if (IsKeyPressed(KEY_LEFT)) framesSpeed--;

        if (framesSpeed > MAX_FRAME_SPEED) framesSpeed = MAX_FRAME_SPEED;
        else if (framesSpeed < MIN_FRAME_SPEED) framesSpeed = MIN_FRAME_SPEED;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            DrawTexture(companheiro, 15, 40, WHITE);
            DrawRectangleLines(15, 40, companheiro.width, companheiro.height, LIME);
            DrawRectangleLines(15 + frameRec.x, 40 + frameRec.y, frameRec.width, frameRec.height, RED);

            DrawText("FRAME SPEED: ", 165, 210, 10, DARKGRAY);
            DrawText(TextFormat("%02i FPS", framesSpeed), 575, 210, 10, DARKGRAY);
            DrawText("PRESS RIGHT/LEFT KEYS to CHANGE SPEED!", 290, 240, 10, DARKGRAY);

            for (int i = 0; i < MAX_FRAME_SPEED; i++)
            {
                if (i < framesSpeed) DrawRectangle(250 + 21*i, 205, 20, 20, RED);
                DrawRectangleLines(250 + 21*i, 205, 20, 20, MAROON);
            }

            DrawTextureRec(companheiro, frameRec, position, WHITE);  // Draw part of the texture

            DrawText("(c) Scarfy sprite by Eiden Marsal", screenWidth - 200, screenHeight - 20, 10, GRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(companheiro);       // Texture unloading

    CloseWindow();                // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
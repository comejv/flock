#include "raylib.h"
#include "state.h"
#include <stddef.h>
#include <stdlib.h>

#ifdef PLUG
void *module_main(void *state)
#else
int main(int argc, char *argv[])
#endif   // ifdef PLUG
{
    // Initialization
    //--------------------------------------------------------------------------------------
#ifdef PLUG
    if (state == NULL)
    {
        state = (simState_t *) malloc(sizeof(simState_t));
    }
#else
    simState_t *state = (simState_t *) malloc(sizeof(simState_t));
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
#endif /* ifdef PLUG */
    //--------------------------------------------------------------------------------------

    PollInputEvents();

    // Main game loop
    while (!WindowShouldClose())   // Detect window close button or ESC key
    {
#ifdef PLUG
        if (IsKeyPressed(KEY_R))   // Return to plug for reload
        {
            return state;
        }
#endif /* ifdef PLUG */
        // Update
        //----------------------------------------------------------------------------------
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(GREEN);

        DrawText("Congrats! You created your first window!", 190, 200, 20, DARKGREEN);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

#ifdef PLUG

    return NULL;
#else
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();   // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    return 0;
#endif /* ifdef PLUG */
}

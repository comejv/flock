#include "entity.h"
#include "movements.h"
#include "raylib.h"
#include "raymath.h"
#include "state.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ENTITIES 1000

#ifdef PLUG
void *module_main(void *old_state)
#else
int main(int argc, char *argv[])
#endif   // ifdef PLUG
{
    // Initialization
    //--------------------------------------------------------------------------------------
    simState_t *state;

#ifdef PLUG
    if (old_state == NULL)
    {
        state = (simState_t *) malloc(sizeof(simState_t));
        state->entities = (entity_t *) malloc(MAX_ENTITIES * sizeof(entity_t));
        for (int i = 0; i < 50; i++)
        {
            state->entities[i] = (entity_t){
                .pos = {GetRandomValue(0, GetScreenWidth()), GetRandomValue(0, GetScreenHeight())},
                .velocity = {GetRandomValue(-5, 5), GetRandomValue(-5, 5)},
                .maxSpeed = GetRandomValue(5, 7),
                .size = {15, 15},
                .col = BLACK};
            state->n_entities++;
        }
        state->groupRadius = 400;
        state->repulsionRadius = 100;
    }
    else
    {
        state = old_state;
    }

    // Done by EndDrawing, required here to stop endless loop when plug reload
    PollInputEvents();
#else
    const int screenWidth = 800;
    const int screenHeight = 800;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Flocking simulator");
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second

    state = (simState_t *) malloc(sizeof(simState_t));
    state->entities = (entity_t *) malloc(MAX_ENTITIES * sizeof(entity_t));

    state->repulsionRadius = 100;
    state->groupRadius = 400;

    for (int i = 0; i < 50; i++)
    {
        state->entities[i] = (entity_t){
            .pos = {GetRandomValue(0, GetScreenWidth()), GetRandomValue(0, GetScreenHeight())},
            .velocity = {GetRandomValue(-5, 5), GetRandomValue(-5, 5)},
            .maxSpeed = GetRandomValue(5, 7),
            .size = {15, 15},
            .col = BLACK};
        state->n_entities++;
    }
#endif /* ifdef PLUG */

    bool showFPS = true;
    bool showEnergy = true;
    bool showHelp = true;
    bool showRadii = true;

    double previousTime = GetTime();
    double currentTime = 0.0;
    float deltaTime = 0.0f;

    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())   // Detect window close button or ESC key
    {
#ifdef PLUG
        if (IsKeyPressed(KEY_R))   // Return to plug for reload
        {
            printf("Reloading plug...\n");
            return state;
        }
#endif /* ifdef PLUG */
        if (IsKeyPressed(KEY_F))
        {
            showFPS = !showFPS;
        }
        else if (IsKeyPressed(KEY_E))
        {
            showEnergy = !showEnergy;
        }
        else if (IsKeyPressed(KEY_P) && state->n_entities < MAX_ENTITIES)
        {
            state->entities[state->n_entities] = (entity_t){
                .pos = {GetRandomValue(0, GetScreenWidth()), GetRandomValue(0, GetScreenHeight())},
                .velocity = {GetRandomValue(-5, 5), GetRandomValue(-5, 5)},
                .maxSpeed = GetRandomValue(4, 10),
                .size = {20, 20},
                .col = BLACK};
            state->n_entities++;
        }
        else if (IsKeyPressed(KEY_SEMICOLON) && state->n_entities > 0)
        {
            state->n_entities--;
        }
        else if (IsKeyPressed(KEY_UP))   // Increase repulsion radius
        {
            state->repulsionRadius += 10.0f;
        }
        else if (IsKeyPressed(KEY_DOWN))   // Decrease repulsion radius
        {
            state->repulsionRadius -= 10.0f;
            if (state->repulsionRadius < 10.0f)
                state->repulsionRadius = 10.0f;   // Prevent negative or too small values
        }
        else if (IsKeyPressed(KEY_LEFT))   // Decrease group radius
        {
            state->groupRadius -= 10.0f;
            if (state->groupRadius < 10.0f)
                state->groupRadius = 10.0f;   // Prevent negative or too small values
        }
        else if (IsKeyPressed(KEY_RIGHT))   // Increase group radius
        {
            state->groupRadius += 10.0f;
        }
        else if (IsKeyPressed(KEY_I))
        {
            showRadii = !showRadii;
        }
        else if (IsKeyPressed(KEY_H))
        {
            showHelp = !showHelp;
        }
        // Update
        //----------------------------------------------------------------------------------
        for (int i = 0; i < state->n_entities; i++)
        {
            entity_t *ent = &state->entities[i];
            ent->pos = Vector2Add(ent->pos, ent->velocity);

            handleBoundariesCollisions(ent, state->repulsionRadius / 2);

            Vector2 meanDirection = {0, 0};
            int count = 0;

            for (int j = 0; j < state->n_entities; j++)
            {
                if (i == j)
                    continue;   // Skip self

                Vector2 distanceVec = Vector2Subtract(ent->pos, state->entities[j].pos);
                float distanceSqr = Vector2LengthSqr(distanceVec);

                float groupRadiusSqr = state->groupRadius * state->groupRadius;
                float repulsionRadiusSqr = state->repulsionRadius * state->repulsionRadius;

                // Repel
                if (distanceSqr < repulsionRadiusSqr)
                {
                    ent->velocity =
                        Vector2Add(ent->velocity,
                                   Vector2Normalize(distanceVec));   // Repel with normalized vector
                }
                // Group (but not too close)
                else if (distanceSqr < groupRadiusSqr)
                {
                    float strength =
                        (distanceSqr - repulsionRadiusSqr) / (groupRadiusSqr - repulsionRadiusSqr);

                    // Apply grouping force scaled by strength
                    ent->velocity = Vector2Add(
                        ent->velocity,
                        Vector2Scale(Vector2Normalize(Vector2Negate(distanceVec)), strength));

                    // Accumulate direction for mean calculation
                    meanDirection = Vector2Add(meanDirection, state->entities[j].velocity);
                    count++;
                }
            }

            if (count > 0)
            {
                // Compute the mean direction
                meanDirection = Vector2Scale(meanDirection, 1.0f / count);
                Vector2 toMeanDirection = Vector2Subtract(meanDirection, ent->velocity);
                float meanDirectionStrength = 0.5f;   // Adjust this value as needed

                // Apply mean direction force
                ent->velocity =
                    Vector2Add(ent->velocity, Vector2Scale(Vector2Normalize(toMeanDirection),
                                                           meanDirectionStrength));
            }

            // Compare length squared with limit squared (optimize)
            if (Vector2LengthSqr(ent->velocity) > ent->maxSpeed * ent->maxSpeed)
            {
                ent->velocity = Vector2Scale(ent->velocity, 0.9f);
            }

            // Drag
            ent->velocity = Vector2Scale(ent->velocity, 0.8f);
        }

        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // Draw entities
        for (int i = 0; i < state->n_entities; i++)
        {
            entity_t *ent = &state->entities[i];
            // Color relative to speed/max speed
            DrawRectangleV(ent->pos, (Vector2){ent->size.x, ent->size.y},
                           ColorFromHSV((1 - (Vector2LengthSqr(ent->velocity) /
                                              (ent->maxSpeed * ent->maxSpeed))) *
                                            220.f,
                                        1, .8f));
        }

        // FPS counter
        if (showFPS)
        {
            DrawText(TextFormat("FPS : %i", (int) (1.0f / deltaTime)), GetScreenWidth() - 180, 20,
                     20, GREEN);
        }
        if (showEnergy)
        {
            float sumSqr = 0;
            for (int i = 0; i < state->n_entities; i++)
            {
                sumSqr += Vector2LengthSqr(state->entities[i].velocity);
            }
            DrawText(TextFormat("Sim energy : %.0f", sqrtf(sumSqr)), GetScreenWidth() - 180, 50, 20,
                     RED);
        }
        if (showRadii)
        {
            DrawText(TextFormat("Repulsion radius : %.0f", state->repulsionRadius), 10,
                     GetScreenHeight() - 60, 20, BLUE);
            DrawText(TextFormat("Group radius : %.0f", state->groupRadius), 10,
                     GetScreenHeight() - 30, 20, BLUE);
        }
        if (showHelp)
        {
            DrawText("Help:", 10, 10, 20, BLUE);
            DrawText("F - Toggle FPS display", 10, 40, 20, BLUE);
            DrawText("E - Toggle Energy display", 10, 70, 20, BLUE);
            DrawText(TextFormat("P - Add entity (max %d)", MAX_ENTITIES), 10, 100, 20, BLUE);
            DrawText("M - Remove last entity", 10, 130, 20, BLUE);
            DrawText("I - Show radii info", 10, 160, 20, BLUE);
            DrawText("L/R arrows - change group radius", 10, 190, 20, BLUE);
            DrawText("U/D arrows - change repulsion radius", 10, 210, 20, BLUE);
        }

        EndDrawing();
        //----------------------------------------------------------------------------------

        if (showFPS)
        {
            currentTime = GetTime();
            deltaTime = (float) (currentTime - previousTime);
            previousTime = currentTime;
        }
    }

#ifdef PLUG
    free(state->entities);
    free(state);
    return NULL;
#else
    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();   // Close window and OpenGL context
    free(state->entities);
    free(state);
    //--------------------------------------------------------------------------------------
    return 0;
#endif /* ifdef PLUG */
}

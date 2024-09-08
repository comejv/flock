#include "entity.h"
#include "movements.h"
#include "raylib.h"
#include "raymath.h"
#include "state.h"
#include "vectors.h"
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ENTITIES       1000
#define DEG_150_IN_RADIANS (5.0f * M_PI / 6.0f)   // 150 degrees in radians

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
        for (int i = 0; i < 30; i++)
        {
            state->entities[i] = createDefaultEntity(GetScreenWidth(), GetScreenHeight());
            state->n_entities++;
        }
        state->groupRadius = 140;
        state->repulsionRadius = 50;
    }
    else
    {
        state = old_state;
    }

    // Done by EndDrawing, required here to stop endless loop when plug reload
    PollInputEvents();
#else
    const int screenWidth = 1600;
    const int screenHeight = 800;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Flocking simulator");
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second

    state = (simState_t *) malloc(sizeof(simState_t));
    state->entities = (entity_t *) malloc(MAX_ENTITIES * sizeof(entity_t));

    state->repulsionRadius = 50;
    state->groupRadius = 140;

    for (int i = 0; i < 30; i++)
    {
        state->entities[i] = createDefaultEntity(GetScreenWidth(), GetScreenHeight());
        state->n_entities++;
    }
#endif /* ifdef PLUG */

    bool showFPS = true;
    bool showEnergy = true;
    bool showHelp = true;
    bool showRadii = true;
    bool showCount = true;

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
            state->entities[state->n_entities] =
                createDefaultEntity(GetScreenWidth(), GetScreenHeight());
            state->n_entities++;
        }
        else if (IsKeyPressed(KEY_SEMICOLON) && state->n_entities > 0)
        {
            state->n_entities--;
        }
        else if (IsKeyPressed(KEY_UP))   // Increase repulsion radius
        {
            state->repulsionRadius += 10.0f;
            if (state->repulsionRadius > state->groupRadius)
            {
                state->repulsionRadius -= 10;
            }
        }
        else if (IsKeyPressed(KEY_DOWN) &&
                 state->repulsionRadius >= 10)   // Decrease repulsion radius
        {
            state->repulsionRadius -= 10.0f;
        }
        else if (IsKeyPressed(KEY_LEFT) && state->groupRadius >= 10)   // Decrease group radius
        {
            state->groupRadius -= 10.0f;
        }
        else if (IsKeyPressed(KEY_RIGHT))   // Increase group radius
        {
            state->groupRadius += 10.0f;
            if (state->groupRadius < state->repulsionRadius)
            {
                state->groupRadius -= 10;
            }
        }
        else if (IsKeyPressed(KEY_I))   // Show radii values
        {
            showRadii = !showRadii;
        }
        else if (IsKeyPressed(KEY_N))   // Show entity count
        {
            showCount = !showCount;
        }
        else if (IsKeyPressed(KEY_H))   // Show keymap
        {
            showHelp = !showHelp;
        }
        // either doesn't register the event or eats the CPU whole
        // else if (IsKeyPressed(KEY_SPACE))
        // {
        //     do
        //     {
        //         WaitTime(1);
        //         PollInputEvents();
        //     } while (!IsKeyPressed(KEY_SPACE));
        // }
        Vector2 groupVec, toCentroid, meanDir;

        // Update
        //----------------------------------------------------------------------------------
        for (int i = 0; i < state->n_entities; i++)
        {
            entity_t *ent = &state->entities[i];
            ent->pos = Vector2Add(ent->pos, ent->velocity);

            handleBoundariesCollisions(ent, state->repulsionRadius / 2);

            Vector2 centroid = Vector2Zero();
            meanDir = Vector2Zero();
            int count = 0;

            for (int j = 0; j < state->n_entities; j++)
            {
                if (i == j)
                    continue;   // Skip self

                Vector2 distanceVec = Vector2Subtract(ent->pos, state->entities[j].pos);
                float distanceSqr = Vector2LengthSqr(distanceVec);

                // Only consider neighbors within group radius and fov
                if (distanceSqr > state->groupRadius * state->groupRadius ||
                    Vector2AngleBetween(distanceVec, ent->velocity) > DEG_150_IN_RADIANS)
                    continue;

                float repulsionRadiusSqr = state->repulsionRadius * state->repulsionRadius;

                // Repel with smoothing
                if (distanceSqr < repulsionRadiusSqr)
                {
                    Vector2 repulsionForce = Vector2Normalize(distanceVec);
                    ent->velocity =
                        Vector2Lerp(ent->velocity, Vector2Add(ent->velocity, repulsionForce),
                                    0.3f);   // Smoothing factor for repulsion
                }
                else
                {
                    // Compute centroid of the flock and mean direction
                    centroid = Vector2Add(centroid, state->entities[j].pos);
                    // Compute mean direction
                    meanDir = Vector2Add(meanDir, state->entities[j].velocity);
                    count++;
                }
            }

            if (count > 0)
            {
                // Compute the centroid
                centroid = Vector2Scale(centroid, 1.0f / count);
                toCentroid = Vector2Normalize(Vector2Subtract(centroid, ent->pos));

                // Compute the mean direction
                meanDir = Vector2Normalize(Vector2Scale(meanDir, 1.0f / count));

                float cohesionFactor = 0.7;   // Towards centroid
                float alignmentFactor = 1;    // Align with the flock
                groupVec = Vector2Add(Vector2Scale(toCentroid, cohesionFactor),
                                      Vector2Scale(meanDir, alignmentFactor));

                // Interpolate the velocity towards the desired velocity
                ent->velocity =
                    Vector2Lerp(ent->velocity, Vector2Add(ent->velocity, groupVec), 0.18);
            }

            // Max speed
            // Clamp velocity to max speed if necessary
            float velLengthSqr = Vector2LengthSqr(ent->velocity);
            int enttSpeedSqr = ENTT_SPEED * ENTT_SPEED;
            if (velLengthSqr > enttSpeedSqr)
            {
                ent->velocity = Vector2Scale(ent->velocity, enttSpeedSqr / velLengthSqr);
            }
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
            Vector2 direction = Vector2Normalize(ent->velocity);

            // Define the three points of the triangle
            Vector2 p1 = (Vector2){ent->pos.x + direction.x * ent->size.x / 2,
                                   ent->pos.y + direction.y * ent->size.x /
                                                    2};   // Tip of the arrow (length)

            // Base vertices (width applied to the perpendicular direction)
            Vector2 p2 =
                (Vector2){ent->pos.x - direction.y * ent->size.y / 2,
                          ent->pos.y + direction.x * ent->size.y / 2};   // One side of the base
            Vector2 p3 = (Vector2){ent->pos.x + direction.y * ent->size.y / 2,
                                   ent->pos.y - direction.x * ent->size.y /
                                                    2};   // The other side of the base

            // Draw the triangle
            // Color relative to speed/max speed
            DrawTriangle(
                p1, p3, p2,
                ColorFromHSV((1 - (Vector2LengthSqr(ent->velocity) / (ENTT_SPEED * ENTT_SPEED))) *
                                 220.f,
                             1, .8f));
            if (!i && showRadii)
            {
                DrawCircleLinesV(ent->pos, state->repulsionRadius, RED);
                DrawCircleLinesV(ent->pos, state->groupRadius, BLUE);
                DrawLineV(ent->pos, Vector2Add(ent->pos, Vector2Scale(groupVec, 100)), MAGENTA);
                DrawLineV(ent->pos, Vector2Add(ent->pos, Vector2Scale(toCentroid, 100)), LIME);
                DrawLineV(ent->pos, Vector2Add(ent->pos, Vector2Scale(meanDir, 100)), MAROON);
            }
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
            DrawText(TextFormat("Group radius : %.0f", state->groupRadius), 10,
                     GetScreenHeight() - 30, 20, BLUE);
            DrawText(TextFormat("Repulsion radius : %.0f", state->repulsionRadius), 10,
                     GetScreenHeight() - 60, 20, BLUE);
            DrawText("Green : to centroid\nMaroon : mean direction\nMagenta : scaled sum", 10,
                     GetScreenHeight() - 140, 20, BLUE);
        }
        if (showCount)
        {
            DrawText(TextFormat("Entity count : %d", state->n_entities), GetScreenWidth() - 180,
                     GetScreenHeight() - 30, 20, BLUE);
        }
        if (showHelp)
        {
            DrawText("F - Toggle FPS display", 10, 10, 20, BLUE);
            DrawText("E - Toggle Energy display", 10, 40, 20, BLUE);
            DrawText("N - Toggle Count display", 10, 70, 20, BLUE);
            DrawText(TextFormat("P - Add entity (max %d)", MAX_ENTITIES), 10, 100, 20, BLUE);
            DrawText("M - Remove last entity", 10, 130, 20, BLUE);
            DrawText("I - Show radii info", 10, 160, 20, BLUE);
            DrawText("L/R arrows - Change group radius", 10, 190, 20, BLUE);
            DrawText("U/D arrows - Change repulsion radius", 10, 220, 20, BLUE);
            DrawText("R - Reload PLUG", 10, 280, 20, BLUE);
            DrawText("ESC - Quit Flock", 10, 310, 20, BLUE);
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

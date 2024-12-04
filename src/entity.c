#include "entity.h"
#include "raymath.h"

entity_t createDefaultEntity(int maxX, int maxY)
{
    // Generate random direction components
    float dirX = (float) GetRandomValue(-1, 1);
    float dirY = (float) GetRandomValue(-1, 1);

    // Calculate the length of the direction vector
    float length = Vector2LengthSqr((Vector2){dirX, dirY});

    // Avoid division by zero and normalize if necessary
    if (length == 0)
    {
        dirX = 1.0f;   // Default direction to avoid zero-length vector
        dirY = 0.0f;
    }
    else
    {
        dirX /= length;
        dirY /= length;
    }

    // Scale the direction to the desired speed
    Vector2 velocity = {dirX * ENTT_SPEED, dirY * ENTT_SPEED};

    return (entity_t){.pos = {GetRandomValue(0, maxX), GetRandomValue(0, maxY)},
                      .velocity = velocity,
                      .size = {ENTT_SIZE_L, ENTT_SIZE_W},
                      .col = BLACK};
}

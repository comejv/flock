#include "movements.h"
#include "raymath.h"

void handleBoundariesCollisions(entity_t *ent, int radius)
{
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Force scale factor
    float steer_strength = 0.1f;

    // Create zero vector for modifications
    Vector2 steering = Vector2Zero();

    // Check proximity to left or right boundary
    Vector2 force = Vector2Zero();
    if (ent->pos.x < radius)
    {
        float distance = radius - ent->pos.x;
        force = Vector2Scale((Vector2){1, 0}, steer_strength * distance);   // Steer right
    }
    else if (ent->pos.x + ent->size.x > screenWidth - radius)
    {
        float distance = (ent->pos.x + ent->size.x) - (screenWidth - radius);
        force = Vector2Scale((Vector2){-1, 0}, steer_strength * distance);   // Steer left
    }
    steering = Vector2Add(steering, force);

    // Check proximity to top or bottom boundary
    if (ent->pos.y < radius)
    {
        float distance = radius - ent->pos.y;
        Vector2 force = Vector2Scale((Vector2){0, 1}, steer_strength * distance);   // Steer down
        steering = Vector2Add(steering, force);
    }
    else if (ent->pos.y + ent->size.y > screenHeight - radius)
    {
        float distance = (ent->pos.y + ent->size.y) - (screenHeight - radius);
        Vector2 force = Vector2Scale((Vector2){0, -1}, steer_strength * distance);   // Steer up
        steering = Vector2Add(steering, force);
    }

    // Apply the steering force to the entity's velocity
    ent->velocity = Vector2Add(ent->velocity, steering);

    // Normalize velocity to limit speed
    float speedSqr = Vector2LengthSqr(ent->velocity);
    if (speedSqr > ent->maxSpeed)
    {
        ent->velocity = Vector2Scale(Vector2Normalize(ent->velocity), ent->maxSpeed);
    }
}

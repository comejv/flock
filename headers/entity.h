#pragma once

#include "raylib.h"
#include <stdint.h>

#define ENTT_SPEED 6
#define ENTT_SIZE_W 20
#define ENTT_SIZE_L 40

struct entity
{
    Vector2 pos;
    Vector2 velocity;
    Vector2 size;
    Color col;
};

typedef struct entity entity_t;

entity_t createDefaultEntity(int maxX, int maxY);

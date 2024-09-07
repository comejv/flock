#pragma once

#include "raylib.h"
#include <stdint.h>
struct entity
{
    Vector2 pos;
    Vector2 velocity;
    int maxSpeed;
    Vector2 size;
    Color col;
};

typedef struct entity entity_t;

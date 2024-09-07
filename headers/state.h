#pragma once

#include "entity.h"
struct simState
{
    entity_t *entities;
    int n_entities;
    float repulsionRadius;
    float groupRadius;
};
typedef struct simState simState_t;

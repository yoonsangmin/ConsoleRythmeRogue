#pragma once

#include "Actor/Actor.h"

class Corridor : public Actor
{
    RTTI_DECLARATIONS(Corridor, Actor)

public:
    Corridor(const Vector2& position);
};
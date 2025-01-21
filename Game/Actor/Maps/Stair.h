#pragma once

#include "Actor/Actor.h"

class Stair : public Actor
{
    RTTI_DECLARATIONS(Stair, Actor)

public:
    Stair(const Vector2& position);
};
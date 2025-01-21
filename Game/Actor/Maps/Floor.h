#pragma once

#include "Actor/Actor.h"

class Floor : public Actor
{
    RTTI_DECLARATIONS(Floor, Actor)

public:
    Floor(const Vector2& position);
};
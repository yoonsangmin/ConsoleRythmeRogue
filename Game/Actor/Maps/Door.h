#pragma once

#include "Actor/Actor.h"

class Door : public Actor
{
    RTTI_DECLARATIONS(Door, Actor)

public:
    Door(const Vector2& position);

    virtual void OnCollisionHit(const Actor& other) override;
};
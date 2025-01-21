#include "Stair.h"

Stair::Stair(const Vector2& position)
    : Actor(L"🪜", position, true, 10, Color::Yellow)
{
    collisionEnabled = true;
    overlapEnabled = true;
    collisionType = ECollision::Item;
}
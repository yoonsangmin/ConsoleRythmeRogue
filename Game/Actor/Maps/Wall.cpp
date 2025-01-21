#include "Wall.h"

Wall::Wall(const Vector2& position)
    : Actor(L"🧱", position, true, 0, Color::BgCyan)
{
    collisionEnabled = true;;
    collisionType = ECollision::Wall;
}

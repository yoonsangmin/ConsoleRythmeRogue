#include "Wall.h"

Wall::Wall(const Vector2& position)
    : Actor(L"🧱", position, true, 0, (Color)((int)Color::White | (int)Color::BgYellow))
{
    collisionEnabled = true;;
    collisionType = ECollision::Wall;
}

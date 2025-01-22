#include "Door.h"
#include "Actor/Player.h"

Door::Door(const Vector2& position)
    : Actor(L"🚪", position, true, 0, (Color)((int)Color::Yellow | (int)Color::BgCyan))
{
    collisionEnabled = true;
    collisionType = ECollision::Door;
}

void Door::OnCollisionHit(Actor& other)
{
    if (other.As<Player>())
    {
        Destroy();
    }
}
#include "Door.h"
#include "Actor/Player.h"

Door::Door(const Vector2& position)
    : Actor(L"🚪", position, true, 0, Color::BgYellow)
{
    collisionEnabled = true;
    collisionType = ECollision::Door;
}

void Door::OnCollisionHit(const Actor& other)
{
    if (other.As<Player>())
    {
        Destroy();
    }
}
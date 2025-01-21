#include "RythmeActor.h"
#include "Level/GameLevel.h"

RythmeActor::RythmeActor(GameLevel* level, const wchar_t* str, const Vector2& position, int hp, int drawOrder, const Color& color)
    : Actor(str, position, true, drawOrder, color), refLevel(level), hp(hp)
{
    // x 축은 짝수 칸에만 설정할 수 있도록 함.
    if (Position().x % 2 == 1)
    {
        this->WarpPosition(Vector2(Position().x - 1, Position().y));
    }

    SetCollisionEnabled(true);
    SetMovable(true);
    collisionType = ECollision::Wall;
}

void RythmeActor::Move(EDirection::Flags direction)
{
    int nextX = Position().x, nextY = Position().y;

    if (direction & EDirection::East)
    {
        nextX += 2;
    }
    if (direction & EDirection::West)
    {
        nextX -= 2;
    }
    if (direction & EDirection::South)
    {
        ++nextY;
    }
    if (direction & EDirection::North)
    {
        --nextY;
    }

    // 이동.
    SetPosition(Vector2(nextX, nextY));
}
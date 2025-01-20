#include "RythmeActor.h"
#include "Level/GameLevel.h"


RythmeActor::RythmeActor(GameLevel* level, const wchar_t* str, const Vector2& position, int hp, int drawOrder, const Color& color)
    : DrawableActor(str, position, drawOrder, color), refLevel(level), hp(hp)
{
    // x 축은 짝수 칸에만 설정할 수 있도록 함.
    if (this->position.x % 2 == 1)
    {
        this->position.x -= 1;
    }
}

bool RythmeActor::Move(EDirection::Flag direction, int amount)
{
    int nextX = position.x, nextY = position.y;

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

    // 이동 가능한지 확인.
    if (CanMove(nextX, nextY))
    {
        position.x = nextX;
        position.y = nextY;

        return true;
    }

    return false;
}

bool RythmeActor::CanMove(int x, int y)
{
    //TODO: 충돌 처리만 해주면 됨.

    return true;
}

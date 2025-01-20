#pragma once

#include "Actor/DrawableActor.h"

struct EDirection
{
    typedef unsigned int Flags;
    enum Flag
    {
        None = 0,
        East = 1 << 0,
        West = 1 << 1,
        South = 1 << 2,
        North = 1 << 3,

        NE = North || East,
        NW = North || West,
        SE = South || East,
        SW = South || West,
    };
};

// 게임 레벨 전방 선언.
class GameLevel;
class RythmeActor : public DrawableActor
{
    RTTI_DECLARATIONS(RythmeActor, DrawableActor)

public:
    // 생성자
    RythmeActor() = delete;
    RythmeActor(GameLevel* level, const wchar_t* str, const Vector2& position, int hp = 6, int drawOrder = 0, const Color& color = Color::White);

protected:
    
    // 이동 함수.
    bool Move(EDirection::Flag direction, int amount = 1);

    bool CanMove(int x, int y);

protected:
    // 체력.
    int hp;

    // 이동 가능한 틱의 번호.
    int tick = 4;

    // 게임 레벨을 참조하는 변수.
    GameLevel* refLevel = nullptr;

};
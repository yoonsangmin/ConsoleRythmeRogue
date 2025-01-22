#pragma once

#include "Actor/Actor.h"

struct EDirection
{
    typedef uint32_t Flags;
    enum Flag
    {
        None = 0,
        East = 1 << 0,
        West = 1 << 1,
        South = 1 << 2,
        North = 1 << 3,

        SE = South | East,
        SW = South | West,
        NE = North | East,
        NW = North | West,
    };

    inline static Flags Random8Direction()
    {
        switch (Random(0, 7))
        {
        case 0:
            return East;
        case 1:
            return West;
        case 2:
            return South;
        case 3:
            return North;
        case 4:
            return SE;
        case 5:
            return SW;
        case 6:
            return NE;
        case 7:
            return NW;
        }

        return East;
    }

    inline static Flags Random4Direction()
    {
        switch (Random(0, 3))
        {
        case 0:
            return East;
        case 1:
            return West;
        case 2:
            return South;
        case 3:
            return North;
        }

        return East;
    }
};

// 게임 레벨 전방 선언.
class GameLevel;
class RythmeActor : public Actor
{
    RTTI_DECLARATIONS(RythmeActor, Actor)

public:
    // 생성자
    RythmeActor() = delete;
    RythmeActor(GameLevel* level, const wchar_t* str, const Vector2& position, int hp = 6, int drawOrder = 0, const Color& color = Color::White);

    virtual void TakeDamage(int damage = 1);

protected:
    
    // 이동 함수.
    void Move(EDirection::Flags direction);

protected:

    // 체력.
    int hp;

    // 게임 레벨을 참조하는 변수.
    GameLevel* refLevel = nullptr;

};
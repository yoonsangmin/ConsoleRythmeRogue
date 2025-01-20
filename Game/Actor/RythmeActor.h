#pragma once

#include "Actor/DrawableActor.h"

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
    // 체력.
    int hp;

    int tick = 4;

    // 게임 레벨을 참조하는 변수.
    GameLevel* refLevel = nullptr;

};
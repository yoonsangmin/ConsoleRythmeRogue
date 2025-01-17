#pragma once

#include <Actor/DrawableActor.h>

// 게임 레벨 전방 선언.
class GameLevel;
class Player : public DrawableActor
{
    RTTI_DECLARATIONS(Player, DrawableActor)

public:
    Player(const Vector2& position, const wchar_t* str, int drawOrder, GameLevel* level);

private:
    // 게임 레벨을 참조하는 변수.
    GameLevel* refLevel = nullptr;
};
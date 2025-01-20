#pragma once

#include "Actor/RythmeActor.h"

// 게임 레벨 전방 선언.
class GameLevel;
class Player : public RythmeActor
{
    RTTI_DECLARATIONS(Player, RythmeActor)

public:
    Player(GameLevel* level, const wchar_t* str, const Vector2& position, int hp = 6, int drawOrder = 0, const Color& color = Color::White);

private:

};
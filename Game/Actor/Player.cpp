#include "Player.h"
#include "Engine/Engine.h"
#include "Level/GameLevel.h"

Player::Player(GameLevel* level, const wchar_t* str, const Vector2& position, int hp, int drawOrder, const Color& color)
    : RythmeActor(level, str, position, hp, drawOrder, color)
{
}

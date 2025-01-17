#include "Player.h"
#include "Engine/Engine.h"
#include "Level/GameLevel.h"

Player::Player(const Vector2& position, const wchar_t* str, int drawOrder, GameLevel* level)
    : DrawableActor(str, drawOrder), refLevel(level)
{
    // 위치 설정.
    this->position = position;

    // 색상 설정.
    color = Color::White;
}
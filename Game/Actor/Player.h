#pragma once

#include "Actor/RythmeActor.h"

// 게임 레벨 전방 선언.
class GameLevel;
class Player : public RythmeActor
{
    RTTI_DECLARATIONS(Player, RythmeActor)

public:
    Player(GameLevel* level, const wchar_t* str, const Vector2& position, int hp = 6, int drawOrder = 0, const Color& color = Color::White);

    virtual void Tick(float deltaTime) override;

private:
    
    // 입력 허용 범위. 0.5를 넘으면 안 됨.
    float inputTolerance = 0.25f;
    float inputToleranceRangeX;
    float inputToleranceRangeY;

    // 이동 후 비활성화, 이후 다음 틱에 이동 가능.
    bool canMove = false;

};
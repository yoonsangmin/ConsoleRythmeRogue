#pragma once

#include "Actor/RythmeActor.h"
#include "Engine/Timer.h"

// 게임 레벨 전방 선언.
class GameLevel;
class Enemy;
class Player : public RythmeActor
{
    RTTI_DECLARATIONS(Player, RythmeActor)

public:
    Player(GameLevel* refLevel, const wchar_t* str, const Vector2& position, int hp = 10, int drawOrder = 10, const Color& color = Color::BrightWhite);

    virtual void Tick(float deltaTime) override;

    virtual void OnCollisionHit(Actor& other) override;
    virtual void OnBeginOverlap(Actor& other) override;

    virtual void Move(EDirection::Flags direction) override;

    virtual void TakeDamage(int damage = 1) override;
    inline Enemy* LastHitEnemy() { return lastHitEnemy; }

private:
    
    // 입력 허용 범위. 0.5를 넘으면 안 됨.
    float inputTolerance = 0.4f;
    float inputToleranceRangeX;
    float inputToleranceRangeY;

    // 이동 후 비활성화, 이후 다음 틱에 이동 가능.
    bool canMove = false;

    Enemy* lastHitEnemy = nullptr;

    Timer enemyTimer;

};
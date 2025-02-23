﻿#pragma once

#include "Actor/RythmeActor.h"

class Enemy : public RythmeActor
{
    RTTI_DECLARATIONS(Enemy, RythmeActor)

public:
    Enemy(GameLevel* gameLevel, const wchar_t* name, const wchar_t* str, const Vector2& position, int hp = 1, const Color& color = Color::Red, int ticksPerMove = 2, int drawOrder = 10);
    ~Enemy();

    virtual void Tick(float deltaTime) override;
    
    wchar_t* GetName();

protected:
    // 이동 방법 결정. 상속받은 적들 마다 다르게 구현.
    virtual EDirection::Flags DecideDirection();

    virtual void OnCollisionHit(Actor& other) override;

protected:

    // 이름.
    wchar_t* name;

    // 이동 체크 관련 변수.
    float previousTimer = 0.0f;
    bool canMove = false;

    int tickCount = 0;
    const int ticksPerMove;

    EDirection::Flags moveDirection = EDirection::None;

};
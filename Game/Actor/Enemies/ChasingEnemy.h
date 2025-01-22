#pragma once

#include "Enemy.h"

class ChasingEnemy : public Enemy
{
    RTTI_DECLARATIONS(ChasingEnemy, Enemy)

public:
    ChasingEnemy(GameLevel* level, const wchar_t* name, const wchar_t* str, const Vector2& position, int hp = 1, const Color& color = Color::Red, bool is4DirectionRandom = true, int ticksPerMove = 2, int drawOrder = 10);

    virtual void BeginPlay() override;

protected:
    virtual EDirection::Flags DecideDirection() override;

    class Player* player = nullptr;

    bool is4DirectionRandom;
};
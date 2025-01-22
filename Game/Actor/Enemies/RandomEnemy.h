#pragma once

#include "Enemy.h"

class RandomEnemy : public Enemy
{
    RTTI_DECLARATIONS(RandomEnemy, Enemy)
        
public:
    RandomEnemy(GameLevel* level, const char* name, const wchar_t* str, const Vector2& position, int hp = 1, const Color& color = Color::Red, bool is4DirectionRandom = true, int ticksPerMove = 2, int drawOrder = 10);

protected:
    virtual EDirection::Flags DecideDirection() override;

private:
    bool is4DirectionRandom;
};
#pragma once

#include "Enemy.h"

class PatrollingEnemy : public Enemy
{
    RTTI_DECLARATIONS(PatrollingEnemy, Enemy)

public:
    PatrollingEnemy(GameLevel* level, const wchar_t* name, const wchar_t* str, const Vector2& position, int hp = 1, const Color& color = Color::Red, bool is4DirectionRandom = true, int ticksPerMove = 2, int drawOrder = 10);

protected:
    virtual EDirection::Flags DecideDirection() override;

private:
    EDirection::Flags patrolDirection = EDirection::South;
    EDirection::Flags nextDirection = EDirection::North;
};
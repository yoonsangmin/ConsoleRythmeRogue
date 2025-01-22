#include "PatrollingEnemy.h"

PatrollingEnemy::PatrollingEnemy(GameLevel* level, const char* name, const wchar_t* str, const Vector2& position, int hp, const Color& color, bool is4DirectionRandom, int ticksPerMove, int drawOrder)
    : Enemy(level, name, str, position, hp, color, ticksPerMove, drawOrder)
{
    patrolDirection = is4DirectionRandom ? EDirection::Random4Direction() : EDirection::Random8Direction();

    nextDirection = EDirection::None;
    if (patrolDirection & EDirection::East)
    {
        nextDirection |= EDirection::West;
    }
    if (patrolDirection & EDirection::West)
    {
        nextDirection |= EDirection::East;
    }
    if (patrolDirection & EDirection::South)
    {
        nextDirection |= EDirection::North;
    }
    if (patrolDirection & EDirection::North)
    {
        nextDirection |= EDirection::South;
    }
}

EDirection::Flags PatrollingEnemy::DecideDirection()
{
    EDirection::Flags temp = patrolDirection;
    patrolDirection = nextDirection;
    nextDirection = temp;
    return patrolDirection;
}

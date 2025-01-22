#include "RandomEnemy.h"

RandomEnemy::RandomEnemy(GameLevel* level, const wchar_t* name, const wchar_t* str, const Vector2& position, int hp, const Color& color, bool is4DirectionRandom, int ticksPerMove, int drawOrder)
    : Enemy(level, name, str, position, hp, color, ticksPerMove, drawOrder), is4DirectionRandom(is4DirectionRandom)
{
}

EDirection::Flags RandomEnemy::DecideDirection()
{
    return is4DirectionRandom ? EDirection::Random4Direction() : EDirection::Random8Direction();
}

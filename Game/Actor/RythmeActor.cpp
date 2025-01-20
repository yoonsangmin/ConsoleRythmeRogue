#include "RythmeActor.h"
#include "Level/GameLevel.h"


RythmeActor::RythmeActor(GameLevel* level, const wchar_t* str, const Vector2& position, int hp, int drawOrder, const Color& color)
    : DrawableActor(str, position, drawOrder, color), refLevel(level), hp(hp)
{
}
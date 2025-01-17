#pragma once

#include "Level/Level.h"
#include "Math/Vector2.h"

// 리듬 로그 레벨
class Player;
class GameLevel : public Level
{
    RTTI_DECLARATIONS(GameLevel, Level)

public:
    GameLevel();

    virtual void Draw() override;

private:

};
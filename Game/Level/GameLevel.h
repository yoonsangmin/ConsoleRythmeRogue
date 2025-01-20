#pragma once

#include "Level/Level.h"
#include "Math/Vector2.h"

// 리듬 로그 레벨
class Player;
class GameLevel : public Level
{
    RTTI_DECLARATIONS(GameLevel, Level)

public:
    GameLevel(float tickPerSecond = 500);

    virtual void Update(float deltaTime) override;
    virtual void Draw() override;

private:
    // 120BPM - 1초에 2비트, 0.5초에 1비트.
    const float tickPerSecond;
    float tickTimer = 0;
    int ticks = 0;
    const int ticksPerBeat = 4;
};
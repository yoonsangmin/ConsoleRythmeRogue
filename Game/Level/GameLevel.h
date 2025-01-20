#pragma once

#include "Level/Level.h"
#include "Math/Vector2.h"

// 리듬 로그 레벨
class Player;
class GameLevel : public Level
{
    RTTI_DECLARATIONS(GameLevel, Level)

public:
    GameLevel(float tickPerSecond = 0.75f);

    virtual void Update(float deltaTime) override;
    virtual void Draw() override;

    // Getter.
    inline float GetTickTimer() const
    { 
        return tickTimer;
    }
    
public:
    // 80BPM - 1비트에 60/80초 - 0.75초.
    const float tickPerSecond;

private:
    float tickTimer = 0;
};
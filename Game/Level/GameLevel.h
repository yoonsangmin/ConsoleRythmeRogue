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

    virtual void Tick(float deltaTime) override;
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

    // 게임 화면 공간.
    Vector2 gameScreen[2];

    // UI 스크린.
    float uiScreenRatio = 0.2f;
    Vector2 uiScreen[2];

    // 비트 표시 스크린.
    float beatScreenRatio = 0.07f;
    Vector2 beatScreen[2];

};
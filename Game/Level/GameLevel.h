﻿#pragma once

#include "Level/Level.h"
#include "Math/Vector2.h"
#include "Engine/Timer.h"

// 리듬 로그 레벨
class Player;
class Map;
class GameLevel : public Level
{
    RTTI_DECLARATIONS(GameLevel, Level)

public:
    GameLevel(float tickPerSecond = 0.5f);
    ~GameLevel();

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;
    virtual void Draw() override;

    // 방 생성.
    void GoToNextLevel();
    void GenerateMap();

    void PlayerDead();

    // Getter.
    inline float GetTickPerSecond() const { return tickPerSecond; }
    inline float GetTickTimer() const { return tickTimer; }
    inline Map* GetMap() { return map; }
    inline bool IsClear() { return isClear; }
private:
    void GameClear();

    void DrawBeatUI();
    void DrawLevelUI();
    void DrawHPUI();
    void DrawEnemyUI();

public:


private:

    // 80BPM - 1비트에 60/80초 - 0.75초.
    // 120BPM - 1비트에 60/120초 - 0.5초.
    const float tickPerSecond;

    float tickTimer = 0;

    // 게임 화면 공간.
    Vector2 gameScreen[2];

    // UI 스크린.
    float uiScreenRatio = 0.12f;
    Vector2 uiScreen[2];

    // 비트 표시 스크린.
    float beatScreenRatio = 0.1f;
    Vector2 beatScreen[2];
    
    // 맵 정보.
    Map* map = nullptr;
    int currentLevel = 1;
    const int END_LEVEL = 3;
    bool isClear = false;
    bool isDead = false;

    // 플레이어.
    Player* refPlayer = nullptr;

    // 종료 타이머.
    Timer quitTimer;

};
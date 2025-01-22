#pragma once

#include "Level/Level.h"
#include "Math/Vector2.h"

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

    // Getter.
    inline float GetTickTimer() const { return tickTimer; }
    inline Map* GetMap() { return map; }
    
private:
    void GameClear();

    void DrawBeatUI();
    void DrawLevelUI();
    void DrawHPUI();
    void DrawEnemyUI();

public:
    // 80BPM - 1비트에 60/80초 - 0.75초.
    // 120BPM - 1비트에 60/120초 - 0.5초.
    const float tickPerSecond;

private:

    float tickTimer = 0;

    // 게임 화면 공간.
    Vector2 gameScreen[2];

    // UI 스크린.
    float uiScreenRatio = 0.2f;
    Vector2 uiScreen[2];

    // 비트 표시 스크린.
    float beatScreenRatio = 0.1f;
    Vector2 beatScreen[2];
    
    // 맵 정보.
    Map* map = nullptr;
    int currentLevel = 1;
    const int END_LEVEL = 1;

    // 플레이어.
    Player* refPlayer = nullptr;

};
#include "GameLevel.h"
#include "Actor/Player.h"
#include "Actor/Enemies/Enemy.h"
#include "Engine/Engine.h"
#include "Actor/Maps/Wall.h"
#include "Actor/Maps/Floor.h"
#include "Actor/Maps/Door.h"
#include "Actor/Maps/Corridor.h"
#include "Actor/Maps/Stair.h"
#include "Actor/Maps/Map.h"

GameLevel::GameLevel(float tickPerSecond)
    : tickPerSecond(tickPerSecond)
{
    // 플레이어 생성.
    Engine::Get().SpawnActor<Player>(this, L"🚶", Vector2(12, 10));

    // 적 생성 테스트.
    Engine::Get().SpawnActor<Enemy>(this, "박쥐", L"🦇", Vector2(10, 10));

    //// 맵 생성 테스트.
    //Engine::Get().SpawnActor<Wall>(Vector2(20, 12));
    //Engine::Get().SpawnActor<Door>(Vector2(18, 12));
    //Engine::Get().SpawnActor<Floor>(Vector2(16, 12));
    //Engine::Get().SpawnActor<Corridor>(Vector2(14, 12));
    //Engine::Get().SpawnActor<Stair>(Vector2(12, 12));

    // 공간 나누기.
    Vector2 screenSize = Engine::Get().ScreenSize();
    int tempX, tempY;
    
    // 비트 스크린. - 아래쪽.
    tempY = (int)((screenSize.y - 1) * (1 - beatScreenRatio));
    beatScreen[0] = { 0, tempY + 1 };
    beatScreen[1] = { screenSize.x - 1, screenSize.y - 1 };

    // UI 스크린. - 오른쪽.
    tempX = (int)((screenSize.x - 1) * (1 - uiScreenRatio));
    uiScreen[0] = { tempX + 1, 0 };
    uiScreen[1] = { screenSize.x - 1, tempY };

    // 게임 스크린.
    gameScreen[0] = { 0, 0 };
    gameScreen[1] = { tempX, tempY };
}

void GameLevel::BeginPlay()
{
    Super::BeginPlay();

    Map map(gameScreen[0], gameScreen[1], 6, Vector2(15, 4));
}

void GameLevel::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    tickTimer += deltaTime;

    if (tickTimer > tickPerSecond)
    {
        tickTimer -= tickPerSecond;
    }
}

void GameLevel::Draw()
{
    Super::Draw();

    // 비트 표시하기.
    for (int y = beatScreen[0].y; y < beatScreen[1].y; ++y)
    {
        int x = (int)(tickTimer / tickPerSecond * (beatScreen[1].x - 1) / 2);
        int reverseX = beatScreen[1].x - 1 - x;

        Engine::Get().Draw(Vector2(x, y), L"｜");
        Engine::Get().Draw(Vector2(x, y), L"｜");

        Engine::Get().Draw(Vector2(reverseX, y), L"｜");
        Engine::Get().Draw(Vector2(reverseX, y), L"｜");
    }
}
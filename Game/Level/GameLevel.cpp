#include "GameLevel.h"
#include "Actor/Player.h"
#include "Engine/Engine.h"
#include "Actor/Maps/Map.h"
#include "Actor/Enemies/Enemy.h"

GameLevel::GameLevel(float tickPerSecond)
    : tickPerSecond(tickPerSecond), currentLevel(1)
{
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

GameLevel::~GameLevel()
{
    delete map;
}

void GameLevel::BeginPlay()
{
    Super::BeginPlay();

    map = new Map(this);
    GenerateMap();
}

void GameLevel::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    tickTimer += deltaTime;

    if (tickTimer > tickPerSecond)
    {
        tickTimer -= tickPerSecond;
    }

    if (isClear || isDead)
    {
        quitTimer.Tick(deltaTime);
        if (quitTimer.IsTimeOut())
        {
            Engine::Get().QuitGame();
        }
    }
}

void GameLevel::Draw()
{
    Super::Draw();

    DrawBeatUI();
    DrawLevelUI();
    DrawHPUI();
    DrawEnemyUI();
}

void GameLevel::GoToNextLevel()
{
    if (currentLevel == END_LEVEL)
    {
        // 종료 처리.
        GameClear();
        return;
    }

    GenerateMap();
    ++currentLevel;
}

void GameLevel::GenerateMap()
{
    if (map == nullptr)
    {
        return;
    }

    map->ClearRooms();
    map->InitializeMap(gameScreen[0], gameScreen[1], 8, Vector2(20, 6));
}

void GameLevel::PlayerDead()
{
    isDead = true;

    // 조작 막기, 잠시 후 종료.
    quitTimer.Reset();
    quitTimer.SetTime(3.0f);
}

void GameLevel::GameClear()
{
    isClear = true;

    // 조작 막기, 잠시 후 종료.
    quitTimer.Reset();
    quitTimer.SetTime(3.0f);
}

void GameLevel::DrawBeatUI()
{
    // 비트 표시하기.
    int x = (int)(tickTimer / tickPerSecond * (beatScreen[1].x - 1) / 2);
    int y = beatScreen[0].y;
    int reverseX = beatScreen[1].x - 1 - x;

    Engine::Get().Draw(Vector2(x, y), L"🌟");

    Engine::Get().Draw(Vector2(reverseX, y), L"🌟");

    Engine::Get().Draw(Vector2(beatScreen[1].x / 2, y), L"🌟", Color::Yellow);
}

void GameLevel::DrawLevelUI()
{
    int x = uiScreen[0].x;
    int y = uiScreen[0].y + 2;
    wchar_t buffer[255];

    if (isClear)
    {
        swprintf(buffer, 255, L"GAME CLEAR!!!");
    }
    else if (isDead)
    {
        swprintf(buffer, 255, L"GAME OVER!!!");
    }
    else
    {
        swprintf(buffer, 255, L"FLOOR: %d/%d", currentLevel, END_LEVEL);
    }
    Engine::Get().Draw(Vector2(x, y), buffer);
}

void GameLevel::DrawHPUI()
{
    if (refPlayer == nullptr)
    {
        FindActorByClass<Player>(refPlayer);
        if (refPlayer == nullptr)
        {
            return;
        }
    }

    int x = uiScreen[0].x;
    int y = uiScreen[0].y + 5;
    wchar_t buffer[255];
    memset(buffer, 0, 255 * sizeof(wchar_t));

    int count = 0;
    int index = 0;
    while (count < refPlayer->GetHP() && index < 255)
    {
        // 짝수 자리에만 찍기.
        if (index & 1)
        {
            buffer[index] = L' ';
            ++index;
            continue;
        }
        buffer[index] = L'❤️';
        ++count;
        ++index;

        if (count == refPlayer->GetHP())
        {
            buffer[index] = '\0';
            Engine::Get().Draw(Vector2(x, y), buffer, Color::BrightRed);
            break;
        }
        if (count % 5 == 0)
        {
            buffer[index] = '\0';
            Engine::Get().Draw(Vector2(x, y), buffer, Color::BrightRed);
            index = 0;
            ++y;
        }
    }
}

void GameLevel::DrawEnemyUI()
{
    if (refPlayer == nullptr)
    {
        FindActorByClass<Player>(refPlayer);
        if (refPlayer == nullptr)
        {
            return;
        }
    }

    Enemy* enemy = refPlayer->LastHitEnemy();

    if (enemy == nullptr || !enemy->IsActive())
    {
        return;
    }

    int x = uiScreen[0].x;
    int y = uiScreen[0].y + 10;
    wchar_t buffer[255];
    memset(buffer, 0, 255 * sizeof(wchar_t));

    Engine::Get().Draw(Vector2(x, y), enemy->GetName());

    x = uiScreen[0].x;
    y = uiScreen[0].y + 12;
    memset(buffer, 0, 255 * sizeof(wchar_t));

    int count = 0;
    int index = 0;
    while (count < enemy->GetHP() && index < 255)
    {
        // 짝수 자리에만 찍기.
        if (index & 1)
        {
            buffer[index] = L' ';
            ++index;
            continue;
        }
        buffer[index] = L'❤️';
        ++count;
        ++index;

        if (count == enemy->GetHP())
        {
            buffer[index] = '\0';
            Engine::Get().Draw(Vector2(x, y), buffer, Color::Red);
            break;
        }
        if (count % 5 == 0)
        {
            buffer[index] = '\0';
            Engine::Get().Draw(Vector2(x, y), buffer, Color::Red);
            index = 0;
            ++y;
        }
    }
}

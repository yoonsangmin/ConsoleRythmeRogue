#include "GameLevel.h"
#include "Actor/Player.h"
#include "Engine/Engine.h"

GameLevel::GameLevel(float tickPerSecond)
    : tickPerSecond(tickPerSecond)
{
    // 플레이어 생성.
    Engine::Get().SpawnActor<Player>(this, L"😊", Vector2(1, 0));
}

void GameLevel::Update(float deltaTime)
{
    Super::Update(deltaTime);

    tickTimer += deltaTime;

    if (tickTimer >= tickPerSecond)
    {
        tickTimer -= tickPerSecond;
        ++ticks;
        ticks %= ticksPerBeat;
    }
}

void GameLevel::Draw()
{
    Super::Draw();
}
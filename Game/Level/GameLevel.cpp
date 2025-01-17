#include "GameLevel.h"
#include "Actor/Player.h"
#include "Engine/Engine.h"

GameLevel::GameLevel()
{
    // 플레이어 생성.
    Engine::Get().SpawnActor<Player>(Vector2(1, 0), L"😊", 12, this);
    Engine::Get().SpawnActor<Player>(Vector2(0, 0), L"😀", 10, this);
    Engine::Get().SpawnActor<Player>(Vector2(2, 0), L"😁", 13, this);
}

void GameLevel::Draw()
{
    Super::Draw();
}
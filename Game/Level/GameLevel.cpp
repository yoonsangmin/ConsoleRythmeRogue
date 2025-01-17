﻿#include "GameLevel.h"
#include "Actor/Player.h"
#include "Engine/Engine.h"

GameLevel::GameLevel()
{
    // 플레이어 생성.
    AddActor(new Player(Vector2(0, 0), this));
}

void GameLevel::Draw()
{
    Super::Draw();
}
﻿#include "Player.h"
#include "Engine/Engine.h"
#include "Level/GameLevel.h"
#include "Actor/Maps/Map.h"
#include "Actor/Maps/Stair.h"
#include "Actor/Maps/Floor.h"
#include "Actor/Enemies/Enemy.h"

Player::Player(GameLevel* refLevel, const wchar_t* str, const Vector2& position, int hp, int drawOrder, const Color& color)
    : RythmeActor(refLevel, str, position, hp, drawOrder, color)
{
    inputToleranceRangeX = refLevel->GetTickPerSecond() - refLevel->GetTickPerSecond() * inputTolerance;
    inputToleranceRangeY = refLevel->GetTickPerSecond() * inputTolerance;

    overlapEnabled = true;
    collisionType = ECollision::Player;
}

void Player::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // ESC 종료.
    if (Engine::Get().GetKeyDown(VK_ESCAPE))
    {
        Engine::Get().QuitGame();
    }

    if (refLevel->IsClear())
    {
        return;
    }

    // 리듬에 맞춰 이동.
    float halfTPS = refLevel->GetTickPerSecond() / 2;
    if (canMove
        && ((refLevel->GetTickTimer() > halfTPS && refLevel->GetTickTimer() >= inputToleranceRangeX)
        || (refLevel->GetTickTimer() <= halfTPS && refLevel->GetTickTimer() <= inputToleranceRangeY)))
    {
        // 동시에 반대 방향이 눌린 경우 무시.
        if (!Engine::Get().GetKeyDown(VK_RIGHT) || !Engine::Get().GetKeyDown(VK_LEFT))
        {
            if (Engine::Get().GetKeyDown(VK_RIGHT))
            {
                Move(EDirection::East);
                canMove = false;
            }
            else if (Engine::Get().GetKeyDown(VK_LEFT))
            {
                Move(EDirection::West);
                canMove = false;
            }
        }
        
        // 좌우로 이동했으면 이동 불가.
        if (canMove && (!Engine::Get().GetKeyDown(VK_DOWN) || !Engine::Get().GetKeyDown(VK_UP)))
        {
            if (Engine::Get().GetKeyDown(VK_DOWN))
            {
                Move(EDirection::South);
                canMove = false;
            }
            else if (Engine::Get().GetKeyDown(VK_UP))
            {
                Move(EDirection::North);
                canMove = false;
            }
        }
    }
    // 틱이 지난 후. 이동 가능하도록 플래그 업데이트.
    else if (!canMove &&
        (refLevel->GetTickTimer() > halfTPS && refLevel->GetTickTimer() < inputToleranceRangeX) ||
        (refLevel->GetTickTimer() <= halfTPS && refLevel->GetTickTimer() > inputToleranceRangeY))
    {
        canMove = true;
    }

    if (lastHitEnemy != nullptr)
    {
        enemyTimer.Tick(deltaTime);
        if (enemyTimer.IsTimeOut())
        {
            lastHitEnemy = nullptr;
        }
    }
}

// 드로우가 실제 이동 후에 불림.
void Player::Draw()
{
    Super::Draw();

    if (Map* map = refLevel->GetMap())
    {
        map->CheckPlayerPosition(Position().x, Position().y);
    }
}

void Player::OnCollisionHit(Actor& other)
{
    if (Enemy* enemy = other.As<Enemy>())
    {
        if (NextPosition() == enemy->Position() && IsMoving())
        {
            enemy->TakeDamage(1);
        }

        lastHitEnemy = enemy;
        enemyTimer.Reset();
        enemyTimer.SetTime(3.0f);
    }
}

void Player::OnBeginOverlap(Actor& other)
{
    if (other.As<Stair>())
    {
        // 방 내려가기.
        refLevel->GoToNextLevel();
        return;
    }
}

void Player::Move(EDirection::Flags direction)
{
    Super::Move(direction);
}

void Player::TakeDamage(int damage)
{
    Super::TakeDamage(damage);

    if (hp <= 0)
    {
        refLevel->PlayerDead();
    }
}

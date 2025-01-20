#include "Player.h"
#include "Engine/Engine.h"
#include "Level/GameLevel.h"

Player::Player(GameLevel* level, const wchar_t* str, const Vector2& position, int hp, int drawOrder, const Color& color)
    : RythmeActor(level, str, position, hp, drawOrder, color)
{
    inputToleranceRangeX = refLevel->tickPerSecond - refLevel->tickPerSecond * inputTolerance;
    inputToleranceRangeY = refLevel->tickPerSecond * inputTolerance;
}

void Player::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // ESC 종료.
    if (Engine::Get().GetKeyDown(VK_ESCAPE))
    {
        Engine::Get().QuitGame();
    }

    // 리듬에 맞춰 이동.
    float halfTPS = refLevel->tickPerSecond / 2;
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
    else if ((refLevel->GetTickTimer() > halfTPS && refLevel->GetTickTimer() < inputToleranceRangeX)
        || (refLevel->GetTickTimer() <= halfTPS && refLevel->GetTickTimer() > inputToleranceRangeY))
    {
        canMove = true;
    }
}
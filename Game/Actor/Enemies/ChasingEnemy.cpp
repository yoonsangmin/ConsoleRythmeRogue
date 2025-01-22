#include "ChasingEnemy.h"
#include "Engine/Engine.h"
#include "Level/Level.h"
#include "Actor/Player.h"

ChasingEnemy::ChasingEnemy(GameLevel* level, const char* name, const wchar_t* str, const Vector2& position, int hp, const Color& color, bool is4DirectionRandom, int ticksPerMove, int drawOrder)
    : Enemy(level, name, str, position, hp, color, ticksPerMove, drawOrder)
{
}

void ChasingEnemy::BeginPlay()
{
    Super::BeginPlay();

    // 플레이어보다 빨리 생성되면 못 찾음.
    Engine::Get().GetMainLevel()->FindActorByClass<Player>(player);
}

EDirection::Flags ChasingEnemy::DecideDirection()
{
    if (player == nullptr)
    {
        // 플레이어 찾기.
        Engine::Get().GetMainLevel()->FindActorByClass<Player>(player);
        
        if (player == nullptr)
        {
            return EDirection::None;
        }
    }

    bool sameX = player->Position().x == Position().x;
    bool sameY = player->Position().y == Position().y;

    // 가로로 이동.
    if (!sameX && sameY)
    {
        if (player->Position().x < Position().x)
        {
            return EDirection::West;
        }

        return EDirection::East;
    }
    // 세로로 이동.
    else if (sameX && !sameY)
    {
        if (player->Position().y < Position().y)
        {
            return EDirection::North;
        }

        return EDirection::South;
    }
    // 랜덤으로 둘 중 하나. 또는 대각선(8방향인 경우) 이동.
    else
    {
        EDirection::Flags flag = EDirection::None;

        int random = is4DirectionRandom ? Random(0, 1) : Random(0, 2);

        if (random == 0 || random == 2)
        {
            if (player->Position().x < Position().x)
            {
                flag |= EDirection::West;
            }
            else
            {
                flag |= EDirection::East;
            }
        }
        if (random == 1 || random == 2)
        {
            if (player->Position().y < Position().y)
            {
                flag |= EDirection::North;
            }
            else
            {
                flag |= EDirection::South;
            }
        }

        return flag;
    }

    return EDirection::Random4Direction();
}

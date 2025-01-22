#include "Enemy.h"
#include "Level/GameLevel.h"
#include "Actor/Player.h"

Enemy::Enemy(GameLevel* refLevel, const wchar_t* name, const wchar_t* str, const Vector2& position, int hp, const Color& color, int ticksPerMove, int drawOrder)
    : RythmeActor(refLevel, str, position, hp, drawOrder, color), ticksPerMove(ticksPerMove)
{
    // 이름 문자열 복사.
    auto length = wcslen(name) + 1;
    this->name = new wchar_t[length];
    wcscpy_s(this->name, length, name);

    collisionType = ECollision::Enemy;
}

Enemy::~Enemy()
{
    delete[] name;
}

void Enemy::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    // 시간이 줄어들면 틱이 됨.
    if (refLevel->GetTickTimer() < previousTimer)
    {
        ++tickCount;
    }
    // 기존 시간 저장.
    previousTimer = refLevel->GetTickTimer();

    if (tickCount >= ticksPerMove)
    {
        tickCount %= ticksPerMove;
        moveDirection = DecideDirection();
        canMove = true;
    }

    if (canMove)
    {
        Move(moveDirection);
        canMove = false;
    }
}

wchar_t* Enemy::GetName()
{
    return name;
}

EDirection::Flags Enemy::DecideDirection()
{
    return EDirection::None;
}

void Enemy::OnCollisionHit(Actor& other)
{
    if (Player* player = other.As<Player>())
    {
        if (NextPosition() == player->Position() && IsMoving())
        {
            player->TakeDamage(1);
        }
    }
}

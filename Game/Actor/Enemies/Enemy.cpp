#include "Enemy.h"
#include "Level/GameLevel.h"

Enemy::Enemy(GameLevel* level, const char* name, const wchar_t* str, const Vector2& position, int hp, int ticksPerMove, int drawOrder, const Color& color)
    : RythmeActor(level, str, position, hp, drawOrder, color), ticksPerMove(ticksPerMove)
{
    // 이름 문자열 복사.
    auto length = strlen(name) + 1;
    this->name = new char[length];
    strcpy_s(this->name, length, name);

}

Enemy::~Enemy()
{
    delete[] name;
}

void Enemy::Update(float deltaTime)
{
    Super::Update(deltaTime);

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
        DecideDirection();
        canMove = true;
    }

    if (canMove)
    {
        Move(moveDirection);
        canMove = false;
    }
}

void Enemy::DecideDirection()
{
    moveDirection = EDirection::Random8Direction();
}

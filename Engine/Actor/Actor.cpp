#include "PreCompiledHeader.h"
#include "Actor.h"
#include "Engine/Engine.h"

Actor::Actor(const wchar_t* image, const Vector2& position, bool isStatic, int drawOrder, const Color& color)
    : isActive(true), isExpired(false), position(position), nextPosition(position), isStatic(isStatic), drawOrder(drawOrder), color(color), isVisible(true)
{
    // 전달받은 문자열 복사.
    auto length = wcslen(image) + 1;
    this->image = new wchar_t[length];
    wcscpy_s(this->image, length, image);

    // 너비 설정(전각 반각 고려).
    width = (int)StringWidth(image);
}

Actor::~Actor()
{
    delete[] image;
}

void Actor::BeginPlay()
{
}

void Actor::Tick(float deltaTime)
{
}

void Actor::ApplyMovement()
{
    position = nextPosition;
}

void Actor::Draw()
{
    if (!isVisible)
    {
        return;
    }

    Engine::Get().Draw(position, image, color, drawOrder);
}

void Actor::SetPosition(const Vector2& newPosition)
{
    nextPosition = newPosition;
}

void Actor::RestorePosition()
{
    nextPosition = position;
}

bool Actor::IsMoving() const
{
    return nextPosition != position;
}

bool Actor::Intersect(const Actor& other)
{
    // AABB(Axis Aligned Bounding Box).

    // 대각선 이동인 경우.
    if (position.x != nextPosition.x && position.y != nextPosition.y)
    {
        // x만 이동하는 경우 충돌하는지?
        if (Intersect(Vector2(nextPosition.x, position.y), width, other.nextPosition, other.width))
        {
            return true;
        }
        // y만 이동하는 경우 충돌하는지?
        if (Intersect(Vector2(position.x, nextPosition.y), width, other.nextPosition, other.width))
        {
            return true;
        }
    }
    // 직선 이동인 경우.
    // 또는 대각선 이동인데 사이에 장애물이 없는 경우.
    // 또는 상대만 움직이는 경우.
    // 나의 다음 좌표랑 다른 액터의 다음 좌표 비교.
    if (Intersect(nextPosition, width, other.nextPosition, other.width))
    {
        return true;
    }

    // 둘 다 움직이고 방향이 반대일 때 - 서로 자리가 바뀌는 경우.
    if (IsMoving() && other.IsMoving()
        && (nextPosition.x - position.x) * (other.nextPosition.x - other.position.x) <= 0
        && (nextPosition.y - position.y) * (other.nextPosition.y - other.position.y) <= 0
        )
    {
        // 방향이 반대인 경우 상대의 현재 위치와 겹치는지 확인.
        return Intersect(nextPosition, width, other.position, other.width);
    }

    // 서로 충돌하지 않고 방향이 반대도 아님.
    return false;
}

bool Actor::Intersect(Vector2 myPosition, int myWidth, Vector2 otherPosition, int otherWidth)
{   
    // 내 x좌표 최소/최대.
    int min = myPosition.x;
    int max = min + myWidth - 1;

    // 다른 액터의 x좌표 최소/최대.
    int otherMin = otherPosition.x;
    int otherMax = otherMin + otherWidth - 1;

    // 다른 액터의 왼쪽 끝 위치가 내 오른쪽 끝 위치를 벗어나면 충돌 안 함.
    if (otherMin > min)
    {
        return false;
    }

    // 다른 액터의 오른쪽 끝 위치가 내 왼쪽 끝 위치보다 작으면 충돌 안 함.
    if (otherMax < min)
    {
        return false;
    }

    // 위의 두 경우가 아니라면 (x좌표는 서로 겹침), y위치 비교.
    return myPosition.y == otherPosition.y;
}

void Actor::OnCollisionHit(Actor& other)
{
}

void Actor::AddNewOverlapActor(Actor* newOverlapActor)
{
    newOverlapActors.insert(newOverlapActor);
}

void Actor::ProcessNewOverlapActors()
{
    // 기존 오버랩된 액터들 중 더 이상 오버랩되지 않는 액터 처리.
    for (Actor* actor : overlappingActors)
    {
        if (!newOverlapActors.count(actor))
        {
            // 더 이상 오버랩되지 않음.
            OnEndOverlap(*actor);
        }
    }

    // 기존 오버랩되지 않던 액터들 중 새로 오버랩된 액터 처리.
    for (Actor* actor : newOverlapActors)
    {
        if (!overlappingActors.count(actor))
        {
            // 새로 오버랩 됨.
            OnBeginOverlap(*actor);
        }
    }

    overlappingActors = newOverlapActors;
    newOverlapActors.clear();
}

void Actor::OnBeginOverlap(Actor& other)
{
}

void Actor::OnEndOverlap(Actor& other)
{
}

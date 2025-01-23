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

bool Actor::IsMoving()
{
    return nextPosition != position;
}

bool Actor::Intersect(const Actor& other)
{
    // AABB(Axis Aligned Bounding Box).

    // 나의 다음 좌표랑 다른 액터의 다음 좌표 비교.
    // 내 x좌표 최소/최대.
    int min = nextPosition.x;
    int max = nextPosition.x + width - 1;

    // 다른 액터의 x좌표 최소/최대.
    int otherMin = other.nextPosition.x;
    int otherMax = other.nextPosition.x + other.width - 1;

    // 다른 액터의 왼쪽 끝 위치가 내 오른쪽 끝 위치를 벗어나면 충돌 안 함.
    if (otherMin > max)
    {
        return false;
    }

    // 다른 액터의 오른쪽 끝 위치가 내 왼쪽 끝 위치보다 작으면 충돌 안 함.
    if (otherMax < min)
    {
        return false;
    }

    // 위의 두 경우가 아니라면 (x좌표는 서로 겹침), y위치 비교.
    return nextPosition.y == other.nextPosition.y;
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

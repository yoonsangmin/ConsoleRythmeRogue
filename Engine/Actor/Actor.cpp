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

bool Actor::Intersect(const Actor& other)
{
    // AABB(Axis Aligned Bounding Box).

    // 내 x좌표 최소/최대.
    int min = position.x;
    int max = position.x + width;

    // 다른 액터의 x좌표 최소/최대.
    int otherMin = other.position.x;
    int otherMax = other.position.x + other.width;

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
    return position.y == other.position.y;
}

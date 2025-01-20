#include "PreCompiledHeader.h"
#include "Actor.h"

Actor::Actor(const Vector2& position)
	: isActive(true), isExpired(false), position(position)
{
}

Actor::~Actor()
{
}

void Actor::BeginPlay()
{
}

void Actor::Tick(float deltaTime)
{
}

void Actor::Draw()
{
}

void Actor::SetPosition(const Vector2& newPosition)
{
    position = newPosition;
}

inline Vector2 Actor::Position() const
{
	return position;
}

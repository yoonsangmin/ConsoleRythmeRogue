#pragma once

#include "Core.h"
#include "RTTI.h"
#include "Math/Vector2.h"

struct ECollision
{
    typedef uint32_t Flag;
    enum Flags
    {
        None = 0,
        Player = 1 << 0,
        Wall = 1 << 1,
        Enemy = 1 << 2,
        Effect = 1 << 3,
        Money = 1 << 4,
        Item = 1 << 5,
    };

    // 충돌 매트릭스.
    static constexpr Flag CollisionMatrix[6] = {
        Wall | Enemy,             // Player는 Wall, Enemy와 충돌.
        Player | Enemy,           // Wall는 Player, Enemy와 충돌.
        Enemy,                    // Enemy는 Player와 충돌.
        None,                     // Effect는 아무것도 충돌하지 않음.
        None,                     // Money는 아무것도 충돌하지 않음.
        None,                     // Item은 아무것도 충돌하지 않음.
    };

    // 오버랩 매트릭스.
    static constexpr Flag OverlapMatrix[6] = {
        None,                     // Player는 아무것도 오버랩하지 않음.
        None,                     // Wall은 아무것도 오버랩하지 않음.
        Effect,                   // Enemy는 Effect와 오버랩.
        Enemy,                    // Effect는 Enemy와 오버랩.
        Player,                   // Money는 Player와 오버랩.
        Player,                   // Item은 Player와 오버랩.
    };
};

class ENGINE_API Actor : public RTTI
{
    // RTTI 선언.
    // 런타임에 빠르게 타입을 확인하기 위한 작업.
    // dynamic_cast 연산자를 활용해야 하는데 성능을 위해 커스텀 버전 사용.
    RTTI_DECLARATIONS(Actor, RTTI)

    // Level 클래스를 friend로 선언.
    // private에 접근이 가능하도록.
    friend class Level;

public:
    Actor(const wchar_t* image = L"", const Vector2& position = Vector2(0, 0), bool isStatic = true, int drawOrder = 0, const Color& color = Color::White);
    virtual ~Actor();

    // 시작 시 한 번 불리는 함수.
    virtual void BeginPlay();

    // 루프 처리 함수.
    virtual void Tick(float deltaTime);
    virtual void Draw();

    virtual void SetPosition(const Vector2& newPosition);

    // 충돌 확인 함수.
    bool Intersect(const Actor& other);

    // Getter/Setter.
    inline Vector2 Position() const { return position; }
    inline bool IsActive() const { return isActive && !isExpired; }
    inline void SetActive(bool active) { isActive = active; }
    inline void Destroy() { isExpired = true; }
    inline int Width() const { return width; }
    inline bool IsVisible() const { return isVisible; }
    inline void SetVisible(bool value) { isVisible = value; }
    inline bool IsCollisionEnabled() const { return collisionEnabled; }
    inline void SetCollisionEnabled(bool value) { collisionEnabled = value; }
    inline bool IsStatic() const { return isStatic; }
    inline bool IsMovable() const { return !isStatic; }

protected:

    // 액터의 위치.
    Vector2 position;

    // 액터가 이동하려는 위치.
    Vector2 nextPosition;

    // ID(해시 - 실제로는) /이름 값.

    // 활성화 상태인지를 나타내는 변수.
    bool isActive;

    // 액터의 제거 요청이 됐는지 여부를 나타내는 변수.
    bool isExpired;

    // 화면에 그릴 문자 값.
    wchar_t* image;

    // 너비(문자열 길이, 전각 반각 고려).
    int width = 0;

    // 그려야 하는 순서.
    int drawOrder = 0;

    // 화면 표시 여부.
    bool isVisible = true;

    // 충돌 처리 여부.
    bool collisionEnabled = true;

    // 움직일 수 없는지 여부.
    bool isStatic = true;

    // 색상 값.
    Color color = Color::White;

    // 콜리전 값.
    ECollision::Flag collision = ECollision::None;
};
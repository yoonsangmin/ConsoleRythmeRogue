#pragma once

#include "Core.h"
#include "RTTI.h"
#include "Math/Vector2.h"
#include "Container/List.h"

struct ECollision
{
    typedef uint8_t Channel;
    enum Channels
    {
        None = -1,
        Player = 0,
        Wall = 1,
        Enemy = 2,
        Effect = 3,
        Item = 4,
        Door = 5,
        Max = 6
    };

    inline static bool CanCollide(Channel channel1, Channel channel2)
    {
        if (channel1 >= 0 && channel1 < Max && channel2 >= 0 && channel2 < Max)
        {
            return CollisionMatrix[channel1][channel2];
        }

        return false;
    }

    inline static bool CanOverlap(Channel channel1, Channel channel2)
    {
        if (channel1 >= 0 && channel1 < Max && channel2 >= 0 && channel2 < Max)
        {
            return OverlapMatrix[channel1][channel2];
        }

        return false;
    }

private:
    // 충돌 매트릭스.
    static constexpr bool CollisionMatrix[Max][Max] = {
        {false, true,  true,  false,  false,  true},      // Player는 Wall, Enemy, Door와 충돌.
        {true,  false, true,  false,  false,  false},     // Wall는 Player, Enemy와 충돌.
        {true,  true,  false, false,  false,  false},     // Enemy는 Wall, Player와 충돌.
        {false,  false,  false,  false, false,  false},   // Effect는 아무것도 충돌하지 않음.
        {false,  false,  false,  false,  false, false},   // Item는 아무것도 충돌하지 않음.
        {true,  false,  true,  false,  false,  false}     // Door는 Player, Enemy와 충돌.
    };

    // 오버랩 매트릭스.
    static constexpr bool OverlapMatrix[Max][Max] = {
        {false, false, false, false, true, false},      // Player는 Item와 오버랩.
        {false, false, false, false, false, false},     // Wall은 아무것도 오버랩하지 않음.
        {false, false, false, true,  false,  false},    // Enemy는 Effect와 오버랩.
        {false, false, true,  false, false, false},     // Effect는 Enemy와 오버랩.
        {true,  false, false,  false,  false, false},   // Item은 Player와 오버랩.
        {false,  false, false, false,  false, false}    // Door는 아무것도 오버랩하지 않음.
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
    virtual void ApplyMovement();
    virtual void Draw();

    // 이동에 사용할 함수.
    virtual void SetPosition(const Vector2& newPosition);
    virtual void RestorePosition();
    virtual bool IsMoving();

    // 충돌 확인 함수.
    bool Intersect(const Actor& other);

    // 충돌 처리 함수.
    virtual void OnCollisionHit(const Actor& other);

    // 오버랩 업데이트.
    void AddNewOverlapActor(Actor* newOverlapActor);
    void ProcessNewOverlapActors();

    // 오버랩 처리 함수.
    virtual void OnBeginOverlap(const Actor& other);
    virtual void OnEndOverlap(const Actor& other);

    // Getter/Setter.
    // 위치 강제 이동.
    inline void WarpPosition(const Vector2& newPosition) { position = newPosition, nextPosition = newPosition; };
    inline Vector2 Position() const { return position; }
    inline bool IsActive() const { return isActive && !isExpired; }
    inline void SetActive(bool active) { isActive = active; }
    inline void Destroy() { isExpired = true; }
    inline int Width() const { return width; }
    inline bool IsVisible() const { return isVisible; }
    inline void SetVisible(bool value) { isVisible = value; }
    inline bool IsCollisionEnabled() const { return collisionEnabled; }
    inline void SetCollisionEnabled(bool value) { collisionEnabled = value; }
    inline bool IsOverlapEnabled() const { return overlapEnabled; }
    inline void SetOverlapEnabled(bool value) { overlapEnabled = value; }
    inline bool IsStatic() const { return isStatic; }
    inline bool IsMovable() const { return !isStatic; }
    inline void SetMovable(bool value) { isStatic = !value; }
    inline ECollision::Channel GetCollisionType() const { return collisionType; }

protected:

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
    bool collisionEnabled = false;

    // 오버랩 처리 여부
    bool overlapEnabled = false;

    // 움직일 수 없는지 여부.
    bool isStatic = true;

    // 색상 값.
    Color color = Color::White;

    // 콜리전 값.
    ECollision::Channel collisionType = ECollision::None;

    // 이전 프레임에 오버랩되고 있던 액터들.
    List<Actor*> overlappingActors;

    // 이번 프레임에 오버랩되는 액터들.
    List<Actor*> newOverlapActors;

private:

    // 액터의 위치.
    Vector2 position;

    // 액터가 이동하려는 위치.
    Vector2 nextPosition;

};
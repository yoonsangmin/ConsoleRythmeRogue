#pragma once

#include "Actor.h"

class ENGINE_API DrawableActor : public Actor
{
    // RTTI.
    RTTI_DECLARATIONS(DrawableActor, Actor)

public:
    DrawableActor(const wchar_t* image = L"", int drawOrder = 0, bool isVisible = true);
    virtual ~DrawableActor();

    virtual void Draw() override;
    virtual void SetPosition(const Vector2& newPosition) override;

    // 충돌 확인 함수.
    bool Intersect(const DrawableActor& other);

    // Getter/Setter.
    inline int Width() const { return width; }
    inline bool IsVisible() const { return isVisible; }
    inline void SetVisible(bool value) { isVisible = value; }

protected:

    // 화면에 그릴 문자 값.
    wchar_t* image;

    // 너비(문자열 길이, 전각 반각 고려).
    int width = 0;

    // 그려야 하는 순서.
    int drawOrder = 0;

    // 화면 표시 여부.
    bool isVisible = true;;

    // 색상 값.
    Color color = Color::White;
};
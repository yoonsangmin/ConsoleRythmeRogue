#pragma once

#include "Core.h"
#include "Math/Vector2.h"

// 화면 버퍼에 기록할 정보.
struct UTF_16_CHAR_INFO
{
    // 문자.
    CHAR_INFO data[2];
    bool is4Byte = false;

    UTF_16_CHAR_INFO()
    {
        data[0].Char.UnicodeChar = L' ';
        data[0].Attributes = 0;
    }

    UTF_16_CHAR_INFO(const wchar_t* str, WORD color)
    {
        data[0].Char.UnicodeChar = str[0];
        data[0].Attributes = color;

        if (Is4ByteUTF16(str[0]))
        {
            data[1].Char.UnicodeChar = str[1];
            data[1].Attributes = color;
            is4Byte = true;
        }
    }

    UTF_16_CHAR_INFO(wchar_t cha, WORD color)
    {
        data[0].Char.UnicodeChar = cha;
        data[0].Attributes = color;
    }

    inline void SetData(const wchar_t* str, WORD color)
    {
        data[0].Char.UnicodeChar = str[0];
        data[0].Attributes = color;

        if (Is4ByteUTF16(str[0]))
        {
            data[1].Char.UnicodeChar = str[1];
            data[1].Attributes = color;
            is4Byte = true;
        }
    }

    inline void SetData(wchar_t cha, WORD color)
    {
        data[0].Char.UnicodeChar = cha;
        data[0].Attributes = color;
    }
};

// 더블 버퍼링 구현에 사용할 화면 버퍼 클래스.
class ScreenBuffer
{
    friend class Engine;

public:
    // 생성자에 크기만 전달하면, 새로운 콘솔 버퍼를 생성.
    ScreenBuffer(const COORD& size);

    // 소멸자.
    ~ScreenBuffer();

    // 커서 타입 설정(커서 감추기 가능) 함수.
    void SetCursorType(CursorType cursorType);

    // 폰트 사이즈 설정.
    void SetConsoleFontSize(short size);

    // 버퍼 지우는 함수.
    void Clear();

    // 콘솔 버퍼에 전체 문자열 버퍼를 쓸 때 사용하는 함수.
    void Draw(UTF_16_CHAR_INFO** buffer);

private:
    // 콘솔 핸들.
    HANDLE buffer;

    // 최근 그렸던 콘솔 크기.
    COORD size;

    // 가로 줄 그리는 데 사용할 임시 버퍼.
    CHAR_INFO* horizontalBuffer;
};
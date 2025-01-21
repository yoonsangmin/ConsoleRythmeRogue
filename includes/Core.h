#pragma once

// @refactor: 경고를 일부러 무시시켰기 때문에 문제가 된다면 고쳐야 함.
#pragma warning (disable : 4172)

#include <iostream>
#include <windows.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

// 색상 열거형.
enum class Color : unsigned short
{
    // 텍스트 색상
    Black = 0,
    Red = FOREGROUND_RED,
    Green = FOREGROUND_GREEN,
    Blue = FOREGROUND_BLUE,
    Yellow = FOREGROUND_RED | FOREGROUND_GREEN,
    Magenta = FOREGROUND_RED | FOREGROUND_BLUE,
    Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
    White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
    BrightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
    BrightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
    BrightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
    BrightYellow = Yellow | FOREGROUND_INTENSITY,
    BrightMagenta = Magenta | FOREGROUND_INTENSITY,
    BrightCyan = Cyan | FOREGROUND_INTENSITY,
    BrightWhite = White | FOREGROUND_INTENSITY,

    // 배경색
    BgBlack = 0,
    BgRed = BACKGROUND_RED,
    BgGreen = BACKGROUND_GREEN,
    BgBlue = BACKGROUND_BLUE,
    BgYellow = BACKGROUND_RED | BACKGROUND_GREEN,
    BgMagenta = BACKGROUND_RED | BACKGROUND_BLUE,
    BgCyan = BACKGROUND_GREEN | BACKGROUND_BLUE,
    BgWhite = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
    BrightBgRed = BACKGROUND_RED | BACKGROUND_INTENSITY,
    BrightBgGreen = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
    BrightBgBlue = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
    BrightBgYellow = BgYellow | BACKGROUND_INTENSITY,
    BrightBgMagenta = BgMagenta | BACKGROUND_INTENSITY,
    BrightBgCyan = BgCyan | BACKGROUND_INTENSITY,
    BrightBgWhite = BgWhite | BACKGROUND_INTENSITY,
};

// 커서의 종류를 설정할 때 사용할 열거형.
enum class CursorType
{
    NoCursor,
    SolidCursor,
    NormalCursor
};

// 메모리 삭제 함수.
template<typename T>
void SafeDelete(T* pointer)
{
    if (pointer != nullptr)
    {
        delete pointer;
        pointer = nullptr;
    }
}

// UTF-16 유니코드 관련.
// 유니코드 블록 범위를 나타내는 구조체
struct UnicodeBlock
{
    uint32_t start;
    uint32_t end;
};

// 이모지가 포함된 유니코드 블록 목록.
const UnicodeBlock emojiBlocks[] =
{
    {0x1F600, 0x1F64F}, // Emoticons
    {0x1F300, 0x1F5FF}, // Miscellaneous Symbols and Pictographs
    {0x1F680, 0x1F6FF}, // Transport and Map Symbols
    {0x1F700, 0x1F77F}, // Alchemical Symbols
    {0x1F780, 0x1F7FF}, // Geometric Shapes Extended
    {0x1F800, 0x1F8FF}, // Supplemental Arrows-C
    {0x1F900, 0x1F9FF}, // Supplemental Symbols and Pictographs
    {0x1FA00, 0x1FA6F}, // Chess Symbols
    {0x1FA70, 0x1FAFF}, // Symbols and Pictographs Extended-A
    {0x2600, 0x26FF},   // Miscellaneous Symbols
    {0x2700, 0x27BF},   // Dingbats
    { 0xD83D, 0xDEAA }, // Door
    // 필요한 경우 추가 블록을 여기에 포함
};

// 주어진 코드 포인트가 이모지 블록에 속하는지 확인하는 함수.
inline bool IsEmojiCodePoint(uint32_t codePoint)
{
    for (const auto& block : emojiBlocks)
    {
        if (codePoint >= block.start && codePoint <= block.end) 
        {
            return true;
        }
    }

    return false;
}

// UTF-16 코드 유닛 두 개로부터 코드 포인트를 계산하는 함수.
inline uint32_t GetCodePoint(wchar_t highSurrogate, wchar_t lowSurrogate) 
{
    return (static_cast<uint32_t>(highSurrogate) - 0xD800) * 0x400 +
        (static_cast<uint32_t>(lowSurrogate) - 0xDC00) + 0x10000;
}

// 전각 문자 범위: U+FF01 ~ U+FF5E.
inline bool IsFullWidth(wchar_t c1) 
{
    return (c1 >= 0xFF01 && c1 <= 0xFF5E);
}

// CJK 문자 범위: U+4E00 ~ U+9FFF.
inline bool IsCJK(wchar_t c1)
{
    return (c1 >= 0x4E00 && c1 <= 0x9FFF);
}

// 주어진 wchar_t 문자가 이모지인지 확인하는 함수.
inline bool IsEmoji(wchar_t c1, wchar_t c2 = 0) 
{
    uint32_t codePoint;
    if (c1 >= 0xD800 && c1 <= 0xDBFF && c2 >= 0xDC00 && c2 <= 0xDFFF) 
    {
        // 서러게이트 페어인 경우
        codePoint = GetCodePoint(c1, c2);
    }
    else 
    {
        // 단일 유닛인 경우
        codePoint = static_cast<uint32_t>(c1);
    }

    return IsEmojiCodePoint(codePoint);
}

// 문자 유형 확인 함수.
// 다른 경우가 있을 수 있음.
inline bool IsFullWidthCharacter(wchar_t c1, wchar_t c2 = 0xFFFF)
{
    if (IsFullWidth(c1))
    {
        return true;
    }
    else if (IsCJK(c1))
    {
        return true;
    }
    else if (IsEmoji(c1, c2)) 
    {
        return true;
    }
    
    return false;
}

inline bool Is4ByteUTF16(wchar_t c) 
{
    // U+D800 - U+DFFF 범위는 서로게이트 쌍 시작 및 종료 범위이므로 4바이트 문자.
    return (c >= 0xD800 && c <= 0xDFFF);
}

// UTF-16 글자 너비 계산.
inline size_t StringWidth(const wchar_t* str)
{
    size_t width = 0;

    for (size_t i = 0; str[i] != '\0'; ++i)
    {
        wchar_t byte = str[i];

        // 4바이트 문자인지 서로게이트 쌍 중 첫 번째만 확인.
        if (Is4ByteUTF16(byte))
        {
            // 전각인가?
            if (IsFullWidthCharacter(str[i], str[i + 1]))
            {
                ++width;
            }

            ++width;
            ++i;
            continue;
        }

        // 전각인가?
        if (IsFullWidthCharacter(str[i]))
        {
            width += 2;
            continue;
        }

        ++width;
    }

    return width;
}

// 로그 함수.
template<typename... T>
void Log(const char* format, T&&... args)
{
    char buffer[1024];
    snprintf(buffer, 1024, format, args...);
    std::cout << buffer;
}

// 랜덤 함수.
inline int Random(int min, int max)
{
    // 차이 구하기.
    int diff = (max - min) + 1;
    return ((diff * rand()) / (RAND_MAX + 1)) + min;
}

// min-max 사이의 랜덤 값을 반환해주는 함수.
inline float RandomPercent(float min, float max)
{
    float random = (float)(rand() / (float)RAND_MAX);
    return random * (max - min) + min;
}

// 메모리 누수 확인할 때 사용하는 함수.
inline void CheckMemoryLeak()
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
}

// 디버깅 용도.
#ifdef _DEBUG
#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define new new
#endif

#if ENGINE_BUILD_DLL
#define ENGINE_API __declspec(dllexport)
#else
#define ENGINE_API __declspec(dllimport)
#endif

/*
 * Virtual Keys, Standard Set
 */
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

#if(_WIN32_WINNT >= 0x0500)
#define VK_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */
#endif /* _WIN32_WINNT >= 0x0500 */

 /*
  * 0x07 : reserved
  */


#define VK_BACK           0x08
#define VK_TAB            0x09

  /*
   * 0x0A - 0x0B : reserved
   */

#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D

   /*
    * 0x0E - 0x0F : unassigned
    */

#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#define VK_KANA           0x15
#define VK_HANGEUL        0x15  /* old name - should be here for compatibility */
#define VK_HANGUL         0x15
#define VK_IME_ON         0x16
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define VK_KANJI          0x19
#define VK_IME_OFF        0x1A

#define VK_ESCAPE         0x1B

#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F

#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F

    /*
     * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
     * 0x3A - 0x40 : unassigned
     * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
     */

#define VK_LWIN           0x5B
#define VK_RWIN           0x5C

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B

#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91

          /*
           * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
           * Used only as parameters to GetAsyncKeyState() and GetKeyState().
           * No other API or message will distinguish left and right keys in this way.
           */
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5
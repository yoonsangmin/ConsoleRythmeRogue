#include "PreCompiledHeader.h"
#include "ScreenBuffer.h"

ScreenBuffer::ScreenBuffer(const COORD& size)
	: size(size)
{
	buffer = CreateConsoleScreenBuffer(GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	if (buffer == INVALID_HANDLE_VALUE)
	{
		__debugbreak();
	}

	SetConsoleScreenBufferSize(buffer, size);
	SMALL_RECT rect = { 0, 0, size.X - 1, size.Y - 1 };
	SetConsoleWindowInfo(buffer, true, &rect);

    horizontalBuffer = new CHAR_INFO[size.X * 2 + 2];
}

ScreenBuffer::~ScreenBuffer()
{
    CloseHandle(buffer);

    delete[] horizontalBuffer;
}

void ScreenBuffer::SetCursorType(CursorType cursorType)
{
    // 1. 커서 속성 구조체 설정.
    CONSOLE_CURSOR_INFO info = { };

    // 타입 별로 구조체 값 설정.
    switch (cursorType)
    {
    case CursorType::NoCursor:
        info.dwSize = 1;
        info.bVisible = FALSE;
        break;

    case CursorType::SolidCursor:
        info.dwSize = 100;
        info.bVisible = TRUE;
        break;

    case CursorType::NormalCursor:
        info.dwSize = 20;
        info.bVisible = TRUE;
        break;
    }

    // 2. 설정.
    SetConsoleCursorInfo(buffer, &info);
}

void ScreenBuffer::SetConsoleFontSize(short size)
{
    // 콘솔 폰트 설정 구조체 초기화.
    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof(fontInfo);

    fontInfo.dwFontSize.Y = size;

    // 새 폰트 설정.
    SetCurrentConsoleFontEx(buffer, FALSE, &fontInfo);
}

void ScreenBuffer::Clear()
{
    COORD position = { 0, 0 };
    DWORD writtenCount = 0;
    FillConsoleOutputCharacter(buffer, ' ', (size.X + 1) * size.Y + 1, position, &writtenCount);
}

void ScreenBuffer::Draw(UTF_16_CHAR_INFO** charInfo)
{
    for (int y = 0; y < size.Y; ++y)
    {
        int count = 0;

        // CHAR_INFO 래퍼 만들면 하나씩 이동해도 됨.
        for (int x = 0; x < size.X; ++x)
        {
            wchar_t firstByte = charInfo[x][y].data[0].Char.UnicodeChar;

            // 버퍼에 적기.
            horizontalBuffer[count] = charInfo[x][y].data[0];
            // 개수 세기.
            ++count;

            // UTF-16 지원 4바이트 UTF-16 문자인 경우 둘 다 넣어주고 카운트도 세어야 함.
            if (Is4ByteUTF16(firstByte))
            {
                wchar_t secondByte = charInfo[x][y].data[1].Char.UnicodeChar;

                horizontalBuffer[count] = charInfo[x][y].data[1];
                ++count;

                // 전각 문자인 경우.
                if (IsFullWidthCharacter(firstByte, secondByte))
                {
                    // 한 칸 무시.
                    ++x;
                }
            }
            // 4바이트가 아닌 UTF-16 문자 중 전각 문자인 경우.
            else if(IsFullWidthCharacter(firstByte))
            {
                // 한 칸 무시.
                ++x;
            }
        }

        COORD bufferPosition = { 0, 0 };
        COORD bufferSize = { count, 1 };
        SMALL_RECT writeRegion = { 0, y, size.X - 1, y };

        // 한 줄씩 출력.
        WriteConsoleOutputW(buffer, horizontalBuffer, bufferSize, bufferPosition, &writeRegion);
    }
}
#include "PreCompiledHeader.h"
#include "Engine.h"
#include <Windows.h>
#include <iostream>

#include "Level/Level.h"
#include "Actor/Actor.h"

#include <time.h>

#include "Render/ScreenBuffer.h"

// 콘솔 창 메시지 콜백 함수.
BOOL WINAPI MessageProcessor(DWORD message)
{
    switch (message)
    {
    case CTRL_CLOSE_EVENT:
        Engine::Get().QuitGame();
        return true;

    default:
        return false;
    }
}

// 스태틱 변수 초기화.
Engine* Engine::instance = nullptr;

Engine::Engine()
	: quit(false), mainLevel(nullptr), screenSize(160, 45)
{
    // 랜덤 시드 설정.
    srand((unsigned int)time(nullptr));

	// 싱글톤 객체 설정.
	instance = this;

    // 타겟 프레임 속도 설정.
    SetTargetFrameRate(60.0f);

    // 화면 그릴 때 사용할 문자 버퍼 초기화.
    imageBuffer = new UTF_16_CHAR_INFO* [screenSize.x];
    
    for (int ix = 0; ix < screenSize.x; ++ix)
    {
        imageBuffer[ix] = new UTF_16_CHAR_INFO[screenSize.y];
    }

    // 버퍼 초기화.
    ClearImageBuffer();

    // 두 개의 버퍼 생성 (버퍼를 번갈아 사용하기 위해-더블 버퍼링).
    COORD size = { (short)screenSize.x, (short)screenSize.y };
    for (int ix = 0; ix < BUFFER_SIZE; ++ix)
    {
        renderTargets[ix] = new ScreenBuffer(size);
        renderTargets[ix]->SetCursorType(CursorType::NoCursor);
        renderTargets[ix]->SetConsoleFontSize(12);
    }

    // 스왑 버퍼.
    Present();

    // 콘솔 창 이벤트 콜백 함수 등록.
    SetConsoleCtrlHandler(MessageProcessor, true);
}

Engine::~Engine()
{
    // 메일 레벨 메모리 해제.
    if (mainLevel != nullptr)
    {
        delete mainLevel;
    }

    // 클리어 버퍼 삭제.
    for (int ix = 0; ix < screenSize.x; ++ix)
    {
        delete[] imageBuffer;
    }
    delete[] imageBuffer;

    // 화면 버퍼 삭제.
    for (int ix = 0; ix < BUFFER_SIZE; ++ix)
    {
        delete renderTargets[ix];
    }
}

void Engine::Run()
{
    // UTF-8 지원 - UTF-16도 지원.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

	// CPU 시계 사용.
	// 시스템 시계 -> 고해상도 카운터. (10000000).
	// 메인보드에 시계가 있음.
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);

	// 시작 시간 및 이전 시간을 위한 변수.
	LARGE_INTEGER time;
	QueryPerformanceCounter(&time);

	int64_t currentTime = time.QuadPart;
	//previousTime = currentTime으로 설정하면 첫 프레임에 Update가 스킵됨.
	int64_t previousTime = currentTime;

	// Game-Loop.
	while (true)
	{
		// 종료 조건.
		if (quit)
		{
			break;
		}

		// 현재 프레임 시간 저장.
		// currentTime = timeGetTime();
		QueryPerformanceCounter(&time);
		currentTime = time.QuadPart;

		// 프레임 시간 계산.
		float deltaTime = static_cast<float>(currentTime - previousTime) / static_cast<float>(frequency.QuadPart);

        // 한 프레임 시간 계산.
        //float targetOneFrameTime = 1.0f / targetFrameRate;

		// 프레임 확인.
		if (deltaTime >= targetOneFrameTime)
		{
			// 입력 처리 (현재 키의 눌림 상태 확인).
			ProcessInput();

            // 업데이트 가능한 상태에서만 프레임 업데이트 처리.
            if (shouldUpdate)
            {
			    Update(deltaTime);
			    Draw();
            }

			// 키 상태 저장.
			SavePreviousKeyState();

			// 이전 프레임 시간 저장.
			previousTime = currentTime;
            
            // 액터 정리 (삭제 요청된 액터들 정리).
            if (mainLevel)
            {
                mainLevel->ProcessAddedAndDestroyedActor();
            }

            // 프레임 활성화.
            shouldUpdate = true;
		}
	}
}

void Engine::LoadLevel(Level* newLevel)
{
    // 기존 레벨이 있다면 삭제 후 교체.

    // 메인 레벨 설정.
    mainLevel = newLevel;
}

void Engine::AddActor(Actor* newActor)
{
    // 예외 처리.
    if (mainLevel == nullptr)
    {
        return;
    }

    // 레벨에 액터 추가.
    shouldUpdate = false;
    mainLevel->AddActor(newActor);
}

void Engine::DestroyActor(Actor* targetActor)
{
    // 예외 처리.
    if (mainLevel == nullptr)
    {
        return;
    }

    // 레벨에 액터 제거.
    shouldUpdate = false;
    targetActor->Destroy();
}

void Engine::SetCursorType(CursorType cursorType)
{
    GetRenderer()->SetCursorType(cursorType);
}

void Engine::Draw(const Vector2& position, const wchar_t* image, Color color)
{
    for (int ix = 0; ix < (int)wcslen(image); ++ix)
    {
        // 잘못된 위치인지 확인.
        int x = position.x;
        int y = position.y;

        if (x >= 0 && x < screenSize.x && y >= 0 && y < screenSize.y)
        {
            // 방금 확인한 문자가 4바이트 UTF-16 문자인 경우 같이 넣어주고 넘어가기.
            if (Is4ByteUTF16(image[ix]))
            {
                wchar_t str[2] = { image[ix], image[ix + 1] };
                imageBuffer[x][y].SetData(str, (unsigned long)color);
                ++ix;
            }
            else
            {
                imageBuffer[x][y].SetData(image[ix], (unsigned long)color);
            }
        }       
    }
}

void Engine::SetTargetFrameRate(float targetFrameRate)
{
    this->targetFrameRate = targetFrameRate;
    targetOneFrameTime = 1.0f / targetFrameRate;
}

bool Engine::GetKey(int key)
{
	return keyState[key].isKeyDown;
}

bool Engine::GetKeyDown(int key)
{
	return keyState[key].isKeyDown && !keyState[key].wasKeyDown;
}

bool Engine::GetKeyUp(int key)
{
	return !keyState[key].isKeyDown && keyState[key].wasKeyDown;
}

void Engine::QuitGame()
{
	// 종료 플래그 설정.
	quit = true;
}

Engine& Engine::Get()
{
	// 싱글톤 객체 반환
	return *instance;
}

void Engine::ProcessInput()
{
	for (int ix = 0; ix < 255; ++ix)
	{
		keyState[ix].isKeyDown = GetAsyncKeyState(ix) & 0x8000 ? true : false;
	}
}

void Engine::Update(float deltaTime)
{
    // 레벨 업데이트.
    if (mainLevel != nullptr)
    {
        mainLevel->Update(deltaTime);
    }
}

void Engine::Draw()
{
    // 화면 지우기.
    Clear();

    // 사용자가 변경한 콘솔 설정 동기화.
    SyncConsoleBufferSettings(renderTargets[currentRenderTargetIndex], renderTargets[frontBufferIndex]);

    // 레벨 그리기
    if (mainLevel != nullptr)
    {
        mainLevel->Draw();
    }

    // 백버퍼에 데이터 쓰기.
    GetRenderer()->Draw(imageBuffer);

    // 프론트<->백 버퍼 교환.
    Present();
}

void Engine::Clear()
{
    ClearImageBuffer();
    GetRenderer()->Clear();
}

void Engine::SavePreviousKeyState()
{
	for (int ix = 0; ix < 255; ++ix)
	{
		keyState[ix].wasKeyDown = keyState[ix].isKeyDown;
	}
}

void Engine::Present()
{
    // Swap Buffer.
    SetConsoleActiveScreenBuffer(GetRenderer()->buffer);
    frontBufferIndex = currentRenderTargetIndex;
    currentRenderTargetIndex = (currentRenderTargetIndex + 1) % BUFFER_SIZE;
}

void Engine::ClearImageBuffer()
{
    // 버퍼 덮어쓰기.
    for (int y = 0; y < screenSize.y; ++y)
    {
        // 버퍼 덮어쓰기.
        for (int x = 0; x < screenSize.x; ++x)
        {
            imageBuffer[x][y].SetData(L' ', 0);
        }
    }
}

void Engine::SyncConsoleBufferSettings(HANDLE sourceBuffer, HANDLE targetBuffer)
{
    // 색상 및 속성 복사.
    CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
    GetConsoleScreenBufferInfo(sourceBuffer, &bufferInfo);
    SetConsoleTextAttribute(targetBuffer, bufferInfo.wAttributes);

    // 폰트 정보 복사.
    CONSOLE_FONT_INFOEX fontInfo = { sizeof(CONSOLE_FONT_INFOEX) };
    GetCurrentConsoleFontEx(sourceBuffer, FALSE, &fontInfo);
    SetCurrentConsoleFontEx(targetBuffer, FALSE, &fontInfo);

    // 커서 설정 복사.
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(sourceBuffer, &cursorInfo);
    SetConsoleCursorInfo(targetBuffer, &cursorInfo);

    // 뷰포트 크기 복사.
    SMALL_RECT viewport = bufferInfo.srWindow;
    SetConsoleWindowInfo(targetBuffer, TRUE, &viewport);

    // 버퍼 크기 복사.
    COORD bufferSize = bufferInfo.dwSize;
    SetConsoleScreenBufferSize(targetBuffer, bufferSize);
}
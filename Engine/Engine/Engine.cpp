#include "PreCompiledHeader.h"
#include "Engine.h"
#include <Windows.h>
#include <iostream>
#include <limits>

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

    // 화면 그릴 때 사용할 문자 버퍼, 순서 버퍼 초기화.
    imageBuffer = new UTF_16_CHAR_INFO* [screenSize.x];
    orderBuffer = new int* [screenSize.x];
    
    for (int ix = 0; ix < screenSize.x; ++ix)
    {
        imageBuffer[ix] = new UTF_16_CHAR_INFO[screenSize.y];
        orderBuffer[ix] = new int[screenSize.y];
        std::memset(orderBuffer[ix], std::numeric_limits<int>::lowest(), screenSize.y * sizeof(int));
    }

    // 버퍼 초기화.
    ClearImageBufferAndOrderBuffer();

    // 두 개의 버퍼 생성 (버퍼를 번갈아 사용하기 위해-더블 버퍼링).
    COORD size = { (short)screenSize.x, (short)screenSize.y };
    for (int ix = 0; ix < BUFFER_SIZE; ++ix)
    {
        renderTargets[ix] = new ScreenBuffer(size);
        renderTargets[ix]->SetCursorType(CursorType::NoCursor);
        renderTargets[ix]->SetConsoleFontSize(16);
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

    // 레벨에 아직 추가되지 않은 액터들 삭제.
    for (Actor* actor : addRequestedActors)
    {
        if (!actor)
        {
            delete actor;
        }
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
	//previousTime = currentTime으로 설정하면 첫 프레임이 스킵됨.
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
		QueryPerformanceCounter(&time);
		currentTime = time.QuadPart;

		// 프레임 시간 계산.
		float deltaTime = static_cast<float>(currentTime - previousTime) / static_cast<float>(frequency.QuadPart);

		// 프레임 확인.
		if (deltaTime >= targetOneFrameTime)
		{
			// 입력 처리 (현재 키의 눌림 상태 확인).
			ProcessInput();

            Tick(deltaTime);
            CheckCollision();
            Draw();

			// 키 상태 저장.
			SavePreviousKeyState();

			// 이전 프레임 시간 저장.
			previousTime = currentTime;
            
            // 액터 정리 (삭제 요청된 액터들 정리), 레벨에 추가.
            ProcessAddedAndDestroyedActor();
		}
	}
}

void Engine::LoadLevel(Level* newLevel)
{
    //TODO: 기존 레벨이 있다면 삭제 후 교체.

    // 메인 레벨 설정.
    mainLevel = newLevel;
}

void Engine::DestroyActor(Actor* targetActor)
{
    // 액터 제거(비활성화).
    targetActor->Destroy();
}

void Engine::ProcessAddedAndDestroyedActor()
{
    // 예외 처리.
    if (!mainLevel)
    {
        return;
    }

    mainLevel->DestroyRequestedActors();
    
    // 레벨에 액터 추가.
    while (addRequestedActors.Size() > 0)
    {
        mainLevel->RequestAddActor(addRequestedActors[0]);
        addRequestedActors[0] = nullptr;
        addRequestedActors.Erase(0);
    }
}

void Engine::SetCursorType(CursorType cursorType)
{
    GetRenderer()->SetCursorType(cursorType);
}

void Engine::Draw(const Vector2& position, const wchar_t* image, Color color, int drawOrder)
{
    bool isPervious4Byte = false;
    int x = position.x;
    int y = position.y;
    int nextX = position.x;

    for (int ix = 0; ix < (int)wcslen(image); ++ix)
    {
        x = nextX;

        bool is4Byte = Is4ByteUTF16(image[ix]);
        bool isFullWidth = IsFullWidthCharacter(image[ix]);

        // 이전 문자가 4바이트 문자였으면 스킵.
        if (isPervious4Byte)
        {
            isPervious4Byte = false;
            continue;
        }

        if (isFullWidth)
        {
            nextX += 2;
        }
        else
        {
            ++nextX;
        }

        // 잘못된 위치인지 확인.
        if (x < 0 || x >= screenSize.x || y < 0 || y >= screenSize.y)
        {
            isPervious4Byte = is4Byte;
            continue;
        }

        // 현재 그릴 곳에 그림이 있는 경우 스킵.
        if (orderBuffer[x][y] > drawOrder)
        {
            isPervious4Byte = is4Byte;
            continue;
        }

        int px = position.x - 1;

        // 한 칸 앞의 문자가 전각인 경우.
        if (px >= 0 && IsFullWidthCharacter(imageBuffer[px][y].data->Char.UnicodeChar))
        {
            // 내가 가려지는 경우.
            if (orderBuffer[px][y] > drawOrder)
            {
                orderBuffer[x][y] = drawOrder;
                isPervious4Byte = is4Byte;
                continue;
            }
            // 내가 기존 문자를 가리는 경우.
            // 앞의 문자를 확인에서 제외시키기 위해 초기화.
            imageBuffer[px][y].SetData(L' ', 0);
        }

        int nx = position.x + 1;

        // 내가 전각 문자인 경우.
        if (IsFullWidthCharacter(image[ix]) && nx < screenSize.x)
        {
            // 내가 가려지는 경우.
            if (orderBuffer[nx][y] > drawOrder)
            {
                orderBuffer[x][y] = drawOrder;
                isPervious4Byte = is4Byte;
                continue;
            }
            // 내가 다음 문자를 가리는 경우.
            orderBuffer[nx][y] = drawOrder;
            // 다음 문자를 확인에서 제외시키기 위해 초기화.
            imageBuffer[nx][y].SetData(L' ', 0);
        }

        // 확인하고 있는 문자가 4바이트 UTF-16 문자인 경우 같이 넣어주고 넘어가기.
        if (is4Byte)
        {
            wchar_t str[2] = { image[ix], image[ix + 1] };
            imageBuffer[x][y].SetData(str, (unsigned long)color);
        }
        else
        {
            imageBuffer[x][y].SetData(image[ix], (unsigned long)color);
        }

        orderBuffer[x][y] = drawOrder;
        isPervious4Byte = is4Byte;
    }
}

void Engine::SetTargetFrameRate(float targetFrameRate)
{
    this->targetFrameRate = targetFrameRate;
    // 한 프레임 시간 계산.
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

void Engine::Tick(float deltaTime)
{
    // 레벨 업데이트.
    if (mainLevel != nullptr)
    {
        mainLevel->Tick(deltaTime);
    }
}

void Engine::CheckCollision()
{
    List<Actor*> actors;
    mainLevel->FindAllActors<Actor>(actors);

    List<Actor*> movableActors;
    List<Actor*> staticActors;
    List<Actor*> noCollisionActors;

    // 필터링.
    for (int ix = 0; ix < actors.Size(); ++ix)
    {
        // 충돌과 오버랩 처리를 모두 안 하는 경우.
        if (!actors[ix]->IsCollisionEnabled() && !actors[ix]->IsOverlapEnabled())
        {
            noCollisionActors.PushBack(actors[ix]);
            continue;
        }

        // 스태틱 액터인 경우.
        if (actors[ix]->IsStatic())
        {
            staticActors.PushBack(actors[ix]);
            continue;
        }

        movableActors.PushBack(actors[ix]);
    }

    // staticActors 충돌 처리.
    for (int ix = 0; ix < movableActors.Size(); ++ix)
    {
        for (int jx = 0; jx < staticActors.Size(); ++jx)
        {
            // 충돌 가능하면 인터섹트 연산.
            if (staticActors[jx]->IsCollisionEnabled() &&
                ECollision::CanCollide(movableActors[ix]->GetCollisionType(), staticActors[jx]->GetCollisionType()) &&
                movableActors[ix]->Intersect(*staticActors[jx]))
            {
                movableActors[ix]->OnCollisionHit(*staticActors[jx]);
                staticActors[jx]->OnCollisionHit(*movableActors[ix]);

                movableActors[ix]->RestorePosition();
            }
        }
    }

    // 충돌 처리.
    for (int ix = 0; ix < movableActors.Size(); ++ix)
    {
        // 각 프레임에서 충돌 발생 여부를 추적.
        bool collisionOccurred = false;
        // 이동 중인 상태 체크. - 무한 루프 방지.
        bool isMoving = movableActors[ix]->IsMoving();

        // movableActors 충돌 처리.
        for (int jx = 0; jx < movableActors.Size(); ++jx)
        {
            // 자신은 무시.
            if (ix == jx)
            {
                continue;
            }

            // 충돌 가능하면 인터섹트 연산.
            if (movableActors[jx]->IsCollisionEnabled() &&
                ECollision::CanCollide(movableActors[ix]->GetCollisionType(), movableActors[jx]->GetCollisionType()) &&
                movableActors[ix]->Intersect(*movableActors[jx]))
            {
                movableActors[ix]->OnCollisionHit(*movableActors[jx]);
                movableActors[jx]->OnCollisionHit(*movableActors[ix]);

                // 충돌이 발생했음을 기록.
                collisionOccurred = true;

                // 위치 되돌리기.
                if (isMoving)
                {
                    movableActors[ix]->RestorePosition();
                    // 위치 변경 발생. 루프 다시 처기하기 위해 탈출.
                    break;
                }
            }
        }

        // 충돌이 발생했고 액터의 위치가 복원된 경우 다른 액터들에 대해 다시 처리해야 함.
        if (collisionOccurred && isMoving)
        {
            // -1로 설정하여 루프가 끝날 때 ++ix가 0부터 시작되도록 함.
            ix = -1;
        }
    }

    // movableActors 이동 처리.
    for (int ix = 0; ix < movableActors.Size(); ++ix)
    {
        movableActors[ix]->ApplyMovement();
    }

    // 스태틱 오버랩 처리.
    for (int ix = 0; ix < staticActors.Size(); ++ix)
    {
        // 오버랩 비활성화 시 무시.
        if (!staticActors[ix]->IsOverlapEnabled())
        {
            continue;
        }

        // 무버블 검사.
        for (int jx = 0; jx < movableActors.Size(); ++jx)
        {
            if (ECollision::CanOverlap(staticActors[ix]->GetCollisionType(), movableActors[jx]->GetCollisionType()) &&
                staticActors[ix]->Intersect(*movableActors[jx]))
            {
                staticActors[ix]->AddNewOverlapActor(movableActors[jx]);
            }
        }

        staticActors[ix]->ProcessNewOverlapActors();
    }

    // 무버블 오버랩 처리.
    for (int ix = 0; ix < movableActors.Size(); ++ix)
    {
        // 오버랩 비활성화 시 무시.
        if (!movableActors[ix]->IsOverlapEnabled())
        {
            continue;
        }

        // 스태틱 검사.
        for (int jx = 0; jx < staticActors.Size(); ++jx)
        {
            if (ECollision::CanOverlap(movableActors[ix]->GetCollisionType(), staticActors[jx]->GetCollisionType()) &&
                movableActors[ix]->Intersect(*staticActors[jx]))
            {
                movableActors[ix]->AddNewOverlapActor(staticActors[jx]);
            }
        }

        // 무버블 검사.
        for (int jx = 0; jx < movableActors.Size(); ++jx)
        {
            // 자기 자신은 무시.
            if (ix == jx)
            {
                continue;
            }

            if (ECollision::CanOverlap(movableActors[ix]->GetCollisionType(), movableActors[jx]->GetCollisionType()) &&
                movableActors[ix]->Intersect(*movableActors[jx]))
            {
                movableActors[ix]->AddNewOverlapActor(movableActors[jx]);
            }
        }

        movableActors[ix]->ProcessNewOverlapActors();
    }
}

void Engine::Draw()
{
    // 화면 지우기.
    Clear();

    //TODO: 사용자가 변경한 콘솔 설정 동기화.
    //SyncConsoleBufferSettings(renderTargets[currentRenderTargetIndex], renderTargets[frontBufferIndex]);
    //SyncConsoleBufferSettings(renderTargets[frontBufferIndex], renderTargets[currentRenderTargetIndex]);

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
    ClearImageBufferAndOrderBuffer();
    //TODO: 버퍼를 통째로 그리고 있기 때문에 필요 없을 것 같음. 삭제해도 되는지 확인 필요.
    // GetRenderer()->Clear();
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

void Engine::ClearImageBufferAndOrderBuffer()
{
    for (int y = 0; y < screenSize.y; ++y)
    {
        for (int x = 0; x < screenSize.x; ++x)
        {
            // 버퍼 덮어쓰기.
            imageBuffer[x][y].SetData(L' ', 0);
            // 깊이 버퍼 초기화
            orderBuffer[x][y] = std::numeric_limits<int>::lowest();
        }
    }
}

// 잘 작동하지 않음...
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
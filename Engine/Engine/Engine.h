#pragma once

#include "Core.h"
#include  "Math/Vector2.h"
#include "Container/List.h"

#define BUFFER_SIZE 2

// 입력 처리를 위한 구조체.
struct KeyState
{
	// 현재 프레임에 키가 눌렸는지 확인.
	bool isKeyDown = false;

	// 이전 프레임에 키가 눌렸었는지 확인.
	bool wasKeyDown = false;
};

// 엔진 클래스.
class Level;
class Actor;
class ScreenBuffer;
class ENGINE_API Engine
{
public:
	Engine();
	virtual ~Engine();

	// 엔진 실행 함수.
	void Run();

    // 레벨 추가 함수.
    void LoadLevel(Level* newLevel);

    //// 액터 추가/삭제 함수.
    //void AddActor(Actor* newActor);
    
    template<typename T, typename... Args>
    inline Actor* SpawnActor(Args&&... args)
    {
        Actor* SpawnedActor = new T(std::forward<Args>(args)...);

        // 추가 요청된 액터 처리.
        addRequestedActors.PushBack(SpawnedActor);

        return SpawnedActor;
    }

    void DestroyActor(Actor* targetActor);

    // 프레임 마지막에 추가, 삭제 처리.
    void ProcessAddedAndDestroyedActor();

    // 화면 좌표 관련 함수.
    void SetCursorType(CursorType cursorType);

    void Draw(const Vector2& position, const wchar_t* image, Color color = Color::White);

    // 화면 크기 반환 함수.
    inline Vector2 ScreenSize() const { return screenSize; }

    // 타겟 프레임 속도 설정 함수.
    void SetTargetFrameRate(float targetFrameRate);

	// 입력 관련 함수.
	bool GetKey(int key);
	bool GetKeyDown(int key);
	bool GetKeyUp(int key);
	
	// 엔진 종료 함수.
	void QuitGame();

	// 싱글톤 객체 접근 함수.
	static Engine& Get();

protected:
	void ProcessInput();			// 입력 처리.
	void SavePreviousKeyState();

	void Update(float deltaTime);	// Tick();
	void Draw();					// Render();

    void Clear();

private:

    // 더블 버퍼링 구현.
    inline ScreenBuffer* GetRenderer() const { return renderTargets[currentRenderTargetIndex]; }
    void Present();                 // 버퍼 교체
    void ClearImageBuffer();					// 화면 지우기.
    void SyncConsoleBufferSettings(HANDLE sourceBuffer, HANDLE targetBuffer);   // 버퍼 정보 동기화

protected:

    // 타겟 프레임 변수(초당 프레임).
    float targetFrameRate = 60.0f;

    // 한 프레임 시간 값(단위: 초).
    float targetOneFrameTime = 0.0f;

	// 종료할 때 설정할 변수.
	bool quit;

	// 키 상태를 저장하는 배열.
	KeyState keyState[255];

	// 싱글톤 구현을 위한 전역 변수 선언.
	static Engine* instance;
	
    // 메인 레벨 변수.
    Level* mainLevel;

    // 추가 요청된 액터들.
    List<Actor*> addRequestedActors;

private:

    // 더블 버퍼링 관련 변수.
    // 화면 크기.
    Vector2 screenSize;

    // 화면 지울 때 사용할 버퍼(Buffer/Blob).
    // UTF-16 서로게이트 페어를 생각해서 x를 2배로 잡음. 그릴 때는 2씩 커지면서 그리기.
    struct UTF_16_CHAR_INFO** imageBuffer = nullptr;

    // 화면 버퍼.
    ScreenBuffer* renderTargets[BUFFER_SIZE];
    // 현재 그리고 있는 버퍼 인덱스.
    int currentRenderTargetIndex = 0;
    // 표시되고 있는 버퍼 인덱스.
    int frontBufferIndex = 1;
};
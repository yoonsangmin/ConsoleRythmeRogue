#pragma once

#include "Core.h"
#include "RTTI.h"
#include "Container/List.h"
#include "Actor/Actor.h"

class ENGINE_API Level : public RTTI
{
    // RTTI 정의.
    RTTI_DECLARATIONS(Level, RTTI)

public:
	Level();
	virtual ~Level();

    // 액터 찾기.
    template<typename T>
    inline void FindAllActors(List<T*>& out)
    {
        for (int ix = 0; ix < actors.Size(); ++ix)
        {
            T* actor = actors[ix]->As<T>();
            if (actor && actors[ix]->IsActive())
            {
                out.PushBack(actor);
            }
        }
    }

	// 액터 추가 함수.
	void RequestAddActor(Actor* newActor);

    // 삭제 요청이 된 액터를 정리하는 함수.
    void DestroyRequestedActors();

	// 루프 처리 함수.
	virtual void Tick(float deltaTime);
	virtual void Draw();

protected:
    //게임 공간에 배치되는 물체(액터) 배열.
    List<Actor*> actors;

};
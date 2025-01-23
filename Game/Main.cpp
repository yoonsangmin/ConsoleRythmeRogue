#include "Engine/Engine.h"
#include "Level/GameLevel.h"

int main()
{
    CheckMemoryLeak();

    Engine engine;
    engine.LoadLevel(new GameLevel());
    engine.Run();
}
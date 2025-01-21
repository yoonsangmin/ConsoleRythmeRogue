#include "Map.h"
#include "Floor.h" 
#include "Wall.h" 
#include "Door.h" 
#include "Corridor.h" 
#include "Stair.h" 

#include "Engine/Engine.h" 

#include <vector>
#include <algorithm>

Map::Map(const Vector2& screenMin, const Vector2& screenMax, int targetCount, const Vector2& maxRoomSize)
{
    CreateRooms(screenMin, screenMax, targetCount, maxRoomSize);
}

void Map::CreateRooms(const Vector2& screenMin, const Vector2& screenMax, int targetCount, const Vector2& maxRoomSize)
{
    int maxRoomSizeX = maxRoomSize.x < MIN_WIDTH ? MIN_WIDTH : maxRoomSize.x;
    int maxRoomSizeY = maxRoomSize.y < MIN_HEIGHT ? MIN_HEIGHT : maxRoomSize.y;

    int mapX = screenMin.x + 2 * ROOM_SPACING;
    int mapY = screenMin.y + ROOM_SPACING;

    int mapWidth = screenMax.x - screenMin.x - 2 * ROOM_SPACING;
    int mapHeight = screenMax.y - screenMin.y - ROOM_SPACING;

    for (int ix = 0; ix < MAX_ATTEMP; ++ix)
    {
        CreateRoom(maxRoomSizeX, maxRoomSizeY, mapX, mapY, mapWidth, mapHeight);

        if (rooms.Size() == targetCount)
        {
            break;
        }
    }
}

bool Map::CreateRoom(int maxRoomSizeX, int maxRoomSizeY, int mapX, int mapY, int mapWidth, int mapHeight)
{
    int width = Random(MIN_WIDTH, maxRoomSizeX);
    int height = Random(MIN_HEIGHT, maxRoomSizeY);

    int x = Random(mapX, mapWidth - width);
    int y = Random(mapY, mapHeight - height);

    // x 무조건 짝수로 처리.
    width = width & 1 ? width - 1 : width;
    x = x & 1 ? x - 1 : x;

    Room room = { x, y, width, height };

    // 콜리전 검사 후 겹치지 않으면 생성.
    if (!CheckCollision(room))
    {
        SpawnMapObjects(room);
        rooms.PushBack(room);

        return true;
    }

    return false;
}

void Map::ClearRooms()
{
}

void Map::SpawnMapObjects(const Room& room)
{
    // 벽 생성.
    for (int y = room.Top() - 1; y <= room.Bottom() + 1; ++y)
    {
        if (y != room.Top() - 1 && y != room.Bottom() + 1)
        {
            Engine::Get().SpawnActor<Wall>(Vector2(room.Left() - 2, y));
            Engine::Get().SpawnActor<Wall>(Vector2(room.Right() + 2, y));

            continue;
        }
        for (int x = room.Left() - 2; x <= room.Right() + 2; x += 2)
        {
            Engine::Get().SpawnActor<Wall>(Vector2(x, y));
        }
    }

    // 바닥 생성.
    for (int y = room.Top(); y <= room.Bottom(); ++y)
    {
        for (int x = room.Left(); x <= room.Right(); x+=2)
        {
            Engine::Get().SpawnActor<Floor>(Vector2(x, y));
        }
    }
}

bool Map::CheckCollision(const Room& newRoom)
{
    for (const Room& room : rooms)
    {
        if (!(newRoom.Right() < room.Left() - 2 * ROOM_SPACING ||
            newRoom.Left() > room.Right() + 2 * ROOM_SPACING ||
            newRoom.Bottom() < room.Top() - ROOM_SPACING ||
            newRoom.Top() > room.Bottom() + ROOM_SPACING))
        {
            return true;
        }
    }

    return false;
}

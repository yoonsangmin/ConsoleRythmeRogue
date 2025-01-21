#pragma once

#include "RTTI.h"
#include <cmath>
#include "Math/Vector2.h"
#include "Container/List.h"

struct Room
{
    // 맨해튼 거리.
    inline int Distance(const Room& other)
    {
        int x1 = Center().x;
        int y1 = Center().y;
        int x2 = other.Center().x;
        int y2 = other.Center().y;
        return std::abs(x1 - x2) + std::abs(y1 - y2);
    }
    inline int Top() const { return y; }
    inline int Bottom() const { return y + height - 1; }
    inline int Right() const { return x + width; }
    inline int Left() const { return x; }
    inline Vector2 Center() const { return Vector2( x + width / 2, y + (height - 1) / 2); }

    int x;
    int y;
    int width;
    int height;
};

class Actor;
class Map : public RTTI
{
    RTTI_DECLARATIONS(Map, RTTI)

public:
    Map(const Vector2& screenMin, const Vector2& screenMax, int targetCount = 5, const Vector2& maxRoomSize = Vector2(10, 5));

    void CreateRooms(const Vector2& screenMin, const Vector2& screenMax, int targetCount, const Vector2& maxRoomSize);
    bool CreateRoom(int maxRoomSizeX, int maxRoomSizeY, int mapX, int mapY, int mapWidth, int mapHeight);

    void ClearRooms();

private:
    void SpawnMapObjects(const Room& room);
    bool CheckCollision(const Room& newRoom);

private:
    const int MIN_WIDTH = 8;
    const int MIN_HEIGHT = 2;
    const int MAX_ATTEMP = 100;
    const int ROOM_SPACING = 3;

    List<Room> rooms;
    List<Actor*> objects;

};
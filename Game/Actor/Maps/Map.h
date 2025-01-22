#pragma once

#include "RTTI.h"

#include <cmath>
#include <set>
#include <vector>
#include <utility>

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
        return std::abs((x1 - x2) / 2) + std::abs(y1 - y2);
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

class GameLevel;
class Actor;
class Corridor;
class Wall;
class Map : public RTTI
{
    RTTI_DECLARATIONS(Map, RTTI)
   
public:
    Map() = delete;
    Map(GameLevel* refLevel);

    void CreateRooms(const Vector2& screenMin, const Vector2& screenMax, int targetCount = 5, const Vector2& maxRoomSize = Vector2(10, 5), float enemySpawnCapability = 0.7f, int enemyMaxPerRoom = 3);
    void ClearRooms();

    void CreateEnemies(float enemySpawnCapability, int enemyMaxPerRoom);
    void SpawnPlayer();

    inline Room GetRoomInfo(int index) { return rooms[index]; }
    inline int RoomsCount(int index) { return rooms.Size(); }

private:
    bool CreateRoom(int maxRoomSizeX, int maxRoomSizeY, int mapX, int mapY, int mapWidth, int mapHeight);
    void CreateEnemiesToRoom(float enemySpawnCapability, int attemptSize, int roomIndex);

    void SpawnFloor();
    void SpawanAllCorridorsAndDoors();
    void SpawnCorridorBetweenRooms(int room1Index, int room2Index);
    void SpawnWalls();
    bool CheckCollision(const Room& newRoom);

    void TrySpawnFloorAt(int x, int y, int roomIndex);
    void TrySpawanCorridorAt(int x, int y);
    void TrySpawnDoorAt(int x, int y, int roomIndex);
    void TrySpawnWallAt(int x, int y, int roomIndex = -1);
    void TrySpawnRandomEnemyAt(int x, int y, int roomIndex);

private:
    GameLevel* refLevel;

    const int MIN_WIDTH = 8;
    const int MIN_HEIGHT = 2;
    const int MAX_ATTEMP = 100;
    const int ROOM_SPACING = 3;

    List<Room> rooms;
    //TODO: 사용하나?
    List<Actor*> objects;

    // 각 방마다의 액터들.
    std::vector<std::vector<Actor*>> roomActors;
    // 통로 액터들.
    List<Corridor*> corridors;
    List<Wall*> corridorWalls;

    // 중복 생성 방지.
    std::set<std::pair<int, int>> mapPositions;
    std::set<std::pair<int, int>> objectPositions;

};
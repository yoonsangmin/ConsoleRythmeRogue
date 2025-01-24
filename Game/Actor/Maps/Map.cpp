#include "Map.h"
#include "Floor.h" 
#include "Wall.h" 
#include "Door.h" 
#include "Corridor.h" 
#include "Stair.h" 
#include "Actor/Player.h"
#include "Actor/Enemies/Enemy.h"
#include "Actor/Enemies/RandomEnemy.h"
#include "Actor/Enemies/PatrollingEnemy.h"
#include "Actor/Enemies/ChasingEnemy.h"

#include "Engine/Engine.h" 

#include <map>
#include <set>

Map::Map(GameLevel* refLevel)
    : refLevel(refLevel)
{
}

void Map::InitializeMap(const Vector2& screenMin, const Vector2& screenMax, int targetCount, const Vector2& maxRoomSize, float enemySpawnCapability, int enemyMaxPerRoom)
{
    // 맵이 이미 있는 경우 생성 중단.
    if (rooms.Size() != 0)
    {
        return;
    }

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
    
    roomActors.resize(rooms.Size(), std::vector<Actor*>());

    // 바닥 생성.
    SpawnFloor();

    // 통로 생성.
    SpawanAllCorridorsAndDoors();
    
    // 벽 생성.
    SpawnWalls();

    // 계단 생성.
    SpawnStair();

    // 적 생성.
    CreateEnemies(enemySpawnCapability, enemyMaxPerRoom);
    
    // 플레이어 생성.
    SpawnPlayer();

    // 모두 가리기.
    SetVisibilityToAllActors(false);

    // 시작방 불 켜기.
    SetVisibilityToRoom(true, 0);
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
        rooms.PushBack(room);

        return true;
    }

    return false;
}

void Map::CreateEnemiesToRoom(float enemySpawnCapability, int attemptSize, int roomIndex)
{
    for (int ix = 0; ix < attemptSize; ++ix)
    {
        float random = RandomPercent(0.0f, 1.0f);

        // 생성 안 함.
        if (enemySpawnCapability < random)
        {
            continue;
        }

        int randomX = Random(rooms[roomIndex].Left(), rooms[roomIndex].Right());
        int randomY = Random(rooms[roomIndex].Top(), rooms[roomIndex].Bottom());
        // X축은 짝수 자리로 만들기.
        randomX = randomX & 1 ? randomX - 1 : randomX;

        TrySpawnRandomEnemyAt(randomX, randomY, roomIndex);
    }
}

void Map::ClearRooms()
{
    for (auto& actors : roomActors)
    {
        for (auto& actor : actors)
        {
            actor->Destroy();
        }
    }

    for (auto& corridor : corridors)
    {
        corridor->Destroy();
    }

    for (auto& wall : corridorWalls)
    {
        wall->Destroy();
    }

    rooms.Clear();

    roomActors.clear();
    corridors.Clear();
    corridorWalls.Clear();
    objects.Clear();

    mapPositions.clear();
    objectPositions.clear();

    litRooms.clear();
}

void Map::SpawnStair()
{
    Room& lastRoom = rooms[rooms.Size() - 1];
    int x = 0;
    int y = 0;
    
    bool canSpawn = false;
    while (!canSpawn)
    {
        x = Random(lastRoom.Left(), lastRoom.Right());
        y = Random(lastRoom.Top(), lastRoom.Bottom());
        // X축은 짝수 자리로 만들기.
        x = x & 1 ? x - 1 : x;


        int dx[] = { 0, 1, 0, -1 };
        int dy[] = { 1, 0, -1, 0 };
        canSpawn = true;
        for (int ix = 0; ix < 4; ++ix)
        {
            for (auto& actor : roomActors[rooms.Size() - 1])
            {
                if (actor->As<Door>() && actor->Position().x == x + dx[ix] && actor->Position().y == y + dy[ix])
                {
                    canSpawn = false;
                    break;
                }
            }

            if (!canSpawn)
            {
                break;
            }
        }
    }

    TrySpawnStairAt(x, y, rooms.Size() - 1);
}

void Map::CreateEnemies(float enemySpawnCapability, int enemyMaxPerRoom)
{
    for (int ix = 1; ix < rooms.Size(); ++ix)
    {
        // 생성.
        CreateEnemiesToRoom(enemySpawnCapability, enemyMaxPerRoom, ix);
    }
}

void Map::SpawnPlayer()
{
    if (rooms.Size() == 0)
    {
        return;
    }

    Vector2 spawnPosition = rooms[0].Center();
    // 짝수 칸으로 옮기기.
    spawnPosition.x = spawnPosition.x & 1 ? spawnPosition.x - 1 : spawnPosition.x;
    if (player == nullptr)
    {
        player = Engine::Get().SpawnActor<Player>(refLevel, L"🚶", spawnPosition)->As<Player>();
    }
    else
    {
        player->WarpPosition(spawnPosition);
    }
}

void Map::SetVisibilityToAllActors(bool value)
{
    // 방 불 끄기.
    for (auto& actors : roomActors)
    {
        for (auto& actor : actors)
        {
            actor->SetVisibility(value);
        }
    }

    litRooms.clear();

    // 복도 불 끄기.
    for (auto& corridor : corridors)
    {
        corridor->SetVisibility(value);
    }
}

void Map::SetVisibilityToRoom(bool value, int roomIndex)
{
    if (litRooms.count(roomIndex))
    {
        return;
    }

    for (auto& actor : roomActors[roomIndex])
    {
        actor->SetVisibility(value);
    }

    litRooms.insert(roomIndex);
}

void Map::CheckPlayerPosition(int x, int y)
{
    for (int ix = 1; ix < roomActors.size(); ++ix)
    {
        for (auto& actor : roomActors[ix])
        {
            if (Floor* floor = actor->As<Floor>())
            {
                if (floor->Position().x == x && floor->Position().y == y)
                {
                    SetVisibilityToRoom(true, ix);
                    return;
                }
            }
        }
    }

    for (auto& object : objects)
    {
        // 맨해튼 거리.
        int distance = std::abs((object->Position().x - x) / 2) + std::abs(object->Position().y - y);

        if (distance <= 1)
        {
            object->SetVisibility(true);
        }
    }

    for (auto& corridor : corridors)
    {
        // 맨해튼 거리.
        int distance = std::abs((corridor->Position().x - x) / 2) + std::abs(corridor->Position().y - y);

        if (distance <= 1)
        {
            corridor->SetVisibility(true);
        }
    }
}

void Map::SpawnFloor()
{
    // 바닥 생성.
    for (int ix = 0; ix < rooms.Size(); ++ix)
    {
        for (int y = rooms[ix].Top(); y <= rooms[ix].Bottom(); ++y)
        {
            for (int x = rooms[ix].Left(); x <= rooms[ix].Right(); x += 2)
            {
                TrySpawnFloorAt(x, y, ix);
            }
        }
    }
}

void Map::SpawanAllCorridorsAndDoors()
{
    // 프림 알고리즘.
    std::multimap<int, std::pair<int, int>> edges;
    std::set<int> connectedRooms;
    List<std::pair<int, int>> selectedEdges;

    int startRoomIndex = 0;
    connectedRooms.insert(startRoomIndex);

    // 시작 방 제외 추가.
    for (int ix = 1; ix < rooms.Size(); ++ix)
    {
        // 거리 순으로 삽입.
        edges.insert({ rooms[startRoomIndex].Distance(rooms[ix]), { startRoomIndex, ix } });
    }

    // 최소 비용 간선만 선택하여 방을 연결.
    while (connectedRooms.size() < rooms.Size())
    {
        auto it = edges.begin();
        while (it != edges.end()) {
            int room1 = it->second.first;
            int room2 = it->second.second;

            if (!connectedRooms.count(room2))
            {
                connectedRooms.insert(room2);
                selectedEdges.PushBack({ room1, room2 });

                // 새로운 방 후보 추가.
                for (int ix = 0; ix < rooms.Size(); ++ix)
                {
                    // 거리 순으로 삽입.
                    edges.insert({ rooms[room2].Distance(rooms[ix]), { room2, ix } });
                }
                
                break;
            }

            ++it;
        }

        // 선택한 간선 제거.
        if (it != edges.end())
        {
            edges.erase(it);
        }
    }
    
    for (int ix = 0; ix < selectedEdges.Size(); ++ix)
    {
        SpawnCorridorBetweenRooms(selectedEdges[ix].first, selectedEdges[ix].second);
    }
}

void Map::SpawnCorridorBetweenRooms(int room1Index, int room2Index)
{
    // 두 방의 중심 좌표 가져오기.
    Room room1 = rooms[room1Index];
    Room room2 = rooms[room2Index];

    Room& leftRoom = room1.Center().x < room2.Center().x ? room1 : room2;
    Room& rightRoom = room1.Center().x < room2.Center().x ? room2 : room1;
    int leftRoomIndex = room1.Center().x < room2.Center().x ? room1Index : room2Index;
    int rightRoomIndex = room1.Center().x < room2.Center().x ? room2Index : room1Index;

    int midX = (leftRoom.Right() + rightRoom.Left()) / 2;
    midX = midX & 1 ? midX - 1 : midX;

    Room& topRoom = room1.Center().y < room2.Center().y ? room1 : room2;
    Room& bottomRoom = room1.Center().y < room2.Center().y ? room2 : room1;
    int topRoomIndex = room1.Center().y < room2.Center().y ? room1Index : room2Index;
    int bottomRoomIndex = room1.Center().y < room2.Center().y ? room2Index : room1Index;
    
    // X축 기준으로 평행할 때.
    if (topRoom.Top() - ROOM_SPACING <= bottomRoom.Top() && bottomRoom.Top() <= topRoom.Bottom() + ROOM_SPACING)
    {
        // 왼쪽 방 가로선.
        for (int x = leftRoom.Right() + 2; x <= midX; x += 2)
        {
            TrySpawanCorridorAt(x, leftRoom.Center().y);
        }
        // 문 생성.
        TrySpawnDoorAt(leftRoom.Right() + 2, leftRoom.Center().y, leftRoomIndex);

        // 오른쪽 방 가로선.
        for (int x = rightRoom.Left() - 2; x >= midX; x -= 2)
        {
            TrySpawanCorridorAt(x, rightRoom.Center().y);
        }
        // 문 생성.
        TrySpawnDoorAt(rightRoom.Left() - 2, rightRoom.Center().y, rightRoomIndex);

        // 세로선.
        for (int y = topRoom.Center().y; y <= bottomRoom.Center().y; ++y)
        {
            TrySpawanCorridorAt(midX, y);
        }
    }
    // Y축 기준으로 평행할 때.
    else if (leftRoom.Left() - 2 * ROOM_SPACING <= rightRoom.Left() && rightRoom.Left() <= leftRoom.Right() + 2 * ROOM_SPACING)
    {
        int topCentX = topRoom.Center().x & 1 ? topRoom.Center().x - 1 : topRoom.Center().x;
        int bottomCentX = bottomRoom.Center().x & 1 ? bottomRoom.Center().x - 1 : bottomRoom.Center().x;

        int midY = (topRoom.Bottom() + bottomRoom.Top()) / 2;

        // 위쪽 방 세로선.
        for (int y = topRoom.Bottom() + 1; y <= midY; ++y)
        {
            TrySpawanCorridorAt(topCentX, y);
        }
        // 문 생성.
        TrySpawnDoorAt(topCentX, topRoom.Bottom() + 1, topRoomIndex);

        // 아래쪽 방 세로선.
        for (int y = bottomRoom.Top() - 1; y >= midY; --y)
        {
            TrySpawanCorridorAt(bottomCentX, y);
        }
        // 문 생성.
        TrySpawnDoorAt(bottomCentX, bottomRoom.Top() - 1, bottomRoomIndex);

        int leftCentX = leftRoom.Center().x & 1 ? leftRoom.Center().x - 1 : leftRoom.Center().x;
        int rightCentX = rightRoom.Center().x & 1 ? rightRoom.Center().x - 1 : rightRoom.Center().x;

        // 가로선.
        for (int x = leftCentX; x <= rightCentX; x += 2)
        {
            TrySpawanCorridorAt(x, midY);
        }
    }
    // ㄱ자 통로 생성. 세 번 꺾이게.
    else
    {
        // 좌상, 우하.
        if (leftRoom.Center().y < rightRoom.Center().y)
        {
            int leftCentX = leftRoom.Center().x & 1 ? leftRoom.Center().x - 1 : leftRoom.Center().x;
            int rightCentX = rightRoom.Center().x & 1 ? rightRoom.Center().x - 1 : rightRoom.Center().x;

            int random = Random(0, 1);
            // 오른쪽->아래쪽.
            if (random)
            {
                int midY = (leftRoom.Bottom() + rightRoom.Top()) / 2;

                for (int x = leftRoom.Right() + 2; x <= rightCentX; x += 2)
                {
                    if (x < midX)
                    {
                        TrySpawanCorridorAt(x, leftRoom.Center().y);
                    }
                    else
                    {
                        TrySpawanCorridorAt(x, midY);
                    }
                }
                // 문 생성.
                TrySpawnDoorAt(leftRoom.Right() + 2, leftRoom.Center().y, leftRoomIndex);

                for (int y = rightRoom.Top() - 1; y >= leftRoom.Center().y; --y)
                {
                    if (y > midY)
                    {
                        TrySpawanCorridorAt(rightCentX, y);
                    }
                    else
                    {
                        TrySpawanCorridorAt(midX, y);
                    }
                }
                // 문 생성.
                TrySpawnDoorAt(rightCentX, rightRoom.Top() - 1, rightRoomIndex);
            }
            // 아래쪽->오른쪽.
            else
            {
                int midY = (leftRoom.Bottom() + rightRoom.Top()) / 2;

                for (int y = leftRoom.Bottom() + 1; y <= rightRoom.Center().y; ++y)
                {
                    if (y < midY)
                    {
                        TrySpawanCorridorAt(leftCentX, y);
                    }
                    else
                    {
                        TrySpawanCorridorAt(midX, y);
                    }
                }
                // 문 생성.
                TrySpawnDoorAt(leftCentX, leftRoom.Bottom() + 1, leftRoomIndex);

                for (int x = rightRoom.Left() - 2; x >= leftCentX; x -= 2)
                {
                    if (x > midX)
                    {
                        TrySpawanCorridorAt(x, rightRoom.Center().y);
                    }
                    else
                    {
                        TrySpawanCorridorAt(x, midY);
                    }
                }
                // 문 생성.
                TrySpawnDoorAt(rightRoom.Left() - 2, rightRoom.Center().y, rightRoomIndex);
            }
        }
        // 좌하, 우상.
        else
        {
            int leftCentX = leftRoom.Center().x & 1 ? leftRoom.Center().x - 1 : leftRoom.Center().x;
            int rightCentX = rightRoom.Center().x & 1 ? rightRoom.Center().x - 1 : rightRoom.Center().x;

            int random = Random(0, 1);
            // 오른쪽->위쪽.
            if (random)
            {
                int midY = (leftRoom.Top() + rightRoom.Bottom()) / 2;

                for (int x = leftRoom.Right() + 2; x <= rightCentX; x += 2)
                {
                    if (x < midX)
                    {
                        TrySpawanCorridorAt(x, leftRoom.Center().y);
                    }
                    else
                    {
                        TrySpawanCorridorAt(x, midY);
                    }
                }
                // 문 생성.
                TrySpawnDoorAt(leftRoom.Right() + 2, leftRoom.Center().y, leftRoomIndex);

                for (int y = rightRoom.Bottom() + 1; y <= leftRoom.Center().y; ++y)
                {
                    if (y < midY)
                    {
                        TrySpawanCorridorAt(rightCentX, y);
                    }
                    else
                    {
                        TrySpawanCorridorAt(midX, y);
                    }
                }
                // 문 생성.
                TrySpawnDoorAt(rightCentX, rightRoom.Bottom() + 1, rightRoomIndex);
            }
            // 위쪽->오른쪽.
            else
            {
                int midY = (leftRoom.Top() + rightRoom.Bottom()) / 2;

                for (int y = leftRoom.Top() - 1; y >= rightRoom.Center().y; --y)
                {
                    if (y > midY)
                    {
                        TrySpawanCorridorAt(leftCentX, y);
                    }
                    else
                    {
                        TrySpawanCorridorAt(midX, y);
                    }
                }
                // 문 생성.
                TrySpawnDoorAt(leftCentX, leftRoom.Top() - 1, leftRoomIndex);

                for (int x = rightRoom.Left() - 2; x >= leftCentX; x -= 2)
                {
                    if (x > midX)
                    {
                        TrySpawanCorridorAt(x, rightRoom.Center().y);
                    }
                    else
                    {
                        TrySpawanCorridorAt(x, midY);
                    }
                }
                // 문 생성.
                TrySpawnDoorAt(rightRoom.Left() - 2, rightRoom.Center().y, rightRoomIndex);
            }
        }
    }
}

void Map::SpawnWalls()
{
    // 벽 생성.
    for (int ix = 0; ix < rooms.Size(); ++ix)
    {
        for (int y = rooms[ix].Top() - 1; y <= rooms[ix].Bottom() + 1; ++y)
        {
            if (y != rooms[ix].Top() - 1 && y != rooms[ix].Bottom() + 1)
            {
                TrySpawnWallAt(rooms[ix].Left() - 2, y, ix);
                TrySpawnWallAt(rooms[ix].Right() + 2, y, ix);

                continue;
            }
            for (int x = rooms[ix].Left() - 2; x <= rooms[ix].Right() + 2; x += 2)
            {
                TrySpawnWallAt(x, y, ix);
            }
        }
    }

    // 통로를 따라서 투명 벽 생성.
    for (int ix = 0; ix < corridors.Size(); ++ix)
    {
        int dx[] = { 0, 2, 0 ,-2 };
        int dy[] = { -1, 0, 1 ,0 };

        for (int jx = 0; jx < 4; ++jx)
        {
            int x = corridors[ix]->Position().x + dx[jx];
            int y = corridors[ix]->Position().y + dy[jx];

            TrySpawnWallAt(x, y);
        }
    }
}

void Map::TrySpawnFloorAt(int x, int y, int roomIndex)
{
    if (!mapPositions.count({ x, y }))
    {
        Actor* actor = Engine::Get().SpawnActor<Floor>(Vector2(x, y));
        if (Floor* floor = actor->As<Floor>())
        {
            mapPositions.insert({ x, y });
            if (roomActors.size() != 0 && roomIndex < roomActors.size() && roomIndex >= 0)
            {
                roomActors[roomIndex].push_back(floor);
            }
        }
    }
}

void Map::TrySpawanCorridorAt(int x, int y)
{
    if (!mapPositions.count({ x, y }))
    {
        Actor* actor = Engine::Get().SpawnActor<Corridor>(Vector2(x, y));
        if (Corridor* corridor = actor->As<Corridor>())
        {
            mapPositions.insert({ x, y });
            corridors.PushBack(corridor);
        }
    }
}

void Map::TrySpawnDoorAt(int x, int y, int roomIndex)
{
    if (!objectPositions.count({ x, y }))
    {
        Actor* actor = Engine::Get().SpawnActor<Door>(Vector2(x, y));
        if (Door* door = actor->As<Door>())
        {
            objectPositions.insert({ x, y });
            if (roomActors.size() != 0 && roomIndex < roomActors.size() && roomIndex >= 0)
            {
                roomActors[roomIndex].push_back(door);
                objects.PushBack(door);
            }
        }
    }
}

void Map::TrySpawnStairAt(int x, int y, int roomIndex)
{
    if (!objectPositions.count({ x, y }))
    {
        Actor* actor = Engine::Get().SpawnActor<Stair>(Vector2(x, y));
        if (Stair* stair = actor->As<Stair>())
        {
            objectPositions.insert({ x, y });
            if (roomActors.size() != 0 && roomIndex < roomActors.size() && roomIndex >= 0)
            {
                roomActors[roomIndex].push_back(stair);
            }
        }
    }
}

void Map::TrySpawnWallAt(int x, int y, int roomIndex)
{
    if (!mapPositions.count({ x, y }))
    {
        Actor* actor = Engine::Get().SpawnActor<Wall>(Vector2(x, y));
        if (Wall* wall = actor->As<Wall>())
        {
            mapPositions.insert({ x, y });
            if (roomActors.size() != 0 && roomIndex < roomActors.size() && roomIndex >= 0)
            {
                roomActors[roomIndex].push_back(wall);
            }
            // 복도용 벽.
            else
            {
                wall->SetVisibility(false);
                corridorWalls.PushBack(wall);
            }
        }
    }
}

void Map::TrySpawnRandomEnemyAt(int x, int y, int roomIndex)
{
    if (!objectPositions.count({ x, y }))
    {
        Actor* actor = nullptr;

        int random = Random(0, 3);
        switch (random)
        {
        case 0:
            actor = Engine::Get().SpawnActor<ChasingEnemy>(refLevel, L"BAT", L"🦇", Vector2(x, y), 2, Color::Blue);
            break;
        case 1:
            actor = Engine::Get().SpawnActor<RandomEnemy>(refLevel, L"GHOST", L"👻", Vector2(x, y), 2, Color::BrightWhite);
            break;
        case 2:
            actor = Engine::Get().SpawnActor<PatrollingEnemy>(refLevel, L"ROBOT", L"🤖", Vector2(x, y), 2, Color::BrightMagenta);
            break;
        case 3:    
        default:
            actor = Engine::Get().SpawnActor<Enemy>(refLevel, L"GUARDIAN", L"🗿", Vector2(x, y), 3, Color::BrightYellow);
            break;
        }

        if (actor != nullptr)
        {
            objectPositions.insert({ x, y });
            if (roomActors.size() != 0 && roomIndex < roomActors.size() && roomIndex >= 0)
            {
                roomActors[roomIndex].push_back(actor);
                objects.PushBack(actor);
            }
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

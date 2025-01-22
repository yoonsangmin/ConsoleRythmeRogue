#include "Map.h"
#include "Floor.h" 
#include "Wall.h" 
#include "Door.h" 
#include "Corridor.h" 
#include "Stair.h" 

#include "Engine/Engine.h" 

#include <map>
#include <set>

Map::Map(const Vector2& screenMin, const Vector2& screenMax, int targetCount, const Vector2& maxRoomSize)
{
    CreateRooms(screenMin, screenMax, targetCount, maxRoomSize);
}

void Map::CreateRooms(const Vector2& screenMin, const Vector2& screenMax, int targetCount, const Vector2& maxRoomSize)
{
    spawnedPositions.clear();

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

    // 바닥 생성.
    SpawnFloor();

    // 통로 생성.
    SpawnCorridorsAndDoors();
    
    // 벽 생성.
    SpawnWalls();
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

void Map::ClearRooms()
{
}

void Map::SpawnFloor()
{
    // 바닥 생성.
    for (const auto& room : rooms)
    {
        for (int y = room.Top(); y <= room.Bottom(); ++y)
        {
            for (int x = room.Left(); x <= room.Right(); x += 2)
            {
                if (!spawnedPositions.count({ x, y }))
                {
                    Engine::Get().SpawnActor<Floor>(Vector2(x, y));
                    spawnedPositions.insert({ x, y });
                }
            }
        }
    }
}

void Map::SpawnCorridorsAndDoors()
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
        SpawnCorridor(selectedEdges[ix].first, selectedEdges[ix].second);
    }
}

void Map::SpawnCorridor(int room1Index, int room2Index)
{
    // 두 방의 중심 좌표 가져오기.
    Room room1 = rooms[room1Index];
    Room room2 = rooms[room2Index];

    Room& leftRoom = room1.Center().x < room2.Center().x ? room1 : room2;
    Room& rightRoom = room1.Center().x < room2.Center().x ? room2 : room1;

    int midX = (leftRoom.Right() + rightRoom.Left()) / 2;
    midX = midX & 1 ? midX - 1 : midX;

    Room& topRoom = room1.Center().y < room2.Center().y ? room1 : room2;
    Room& bottomRoom = room1.Center().y < room2.Center().y ? room2 : room1;
    
    // X축 기준으로 평행할 때.
    if (topRoom.Top() - ROOM_SPACING <= bottomRoom.Top() && bottomRoom.Top() <= topRoom.Bottom() + ROOM_SPACING)
    {
        // 왼쪽 방 가로선.
        for (int x = leftRoom.Right() + 2; x <= midX; x += 2)
        {
            if (!spawnedPositions.count({ x, leftRoom.Center().y }))
            {
                Engine::Get().SpawnActor<Corridor>(Vector2(x, leftRoom.Center().y));
                spawnedPositions.insert({ x, leftRoom.Center().y });
            }
        }
        // 문 생성.
        if (!doorPositions.count({ leftRoom.Right() + 2, leftRoom.Center().y }))
        {
            Engine::Get().SpawnActor<Door>(Vector2(leftRoom.Right() + 2, leftRoom.Center().y));
            doorPositions.insert({ leftRoom.Right() + 2, leftRoom.Center().y });
        }

        // 오른쪽 방 가로선.
        for (int x = rightRoom.Left() - 2; x >= midX; x -= 2)
        {
            if (!spawnedPositions.count({ x, rightRoom.Center().y }))
            {
                Engine::Get().SpawnActor<Corridor>(Vector2(x, rightRoom.Center().y));
                spawnedPositions.insert({ x, rightRoom.Center().y });
            }
        }
        // 문 생성.
        if (!doorPositions.count({ rightRoom.Left() - 2, rightRoom.Center().y }))
        {
            Engine::Get().SpawnActor<Door>(Vector2(rightRoom.Left() - 2, rightRoom.Center().y));
            doorPositions.insert({ rightRoom.Left() - 2, rightRoom.Center().y });
        }

        // 세로선.
        for (int y = topRoom.Center().y; y <= bottomRoom.Center().y; ++y)
        {
            if (!spawnedPositions.count({ midX, y }))
            {
                Engine::Get().SpawnActor<Corridor>(Vector2(midX, y));
                spawnedPositions.insert({ midX, y });
            }
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
            if (!spawnedPositions.count({ topCentX, y }))
            {
                Engine::Get().SpawnActor<Corridor>(Vector2(topCentX, y));
                spawnedPositions.insert({ topCentX, y });
            }
        }
        // 문 생성.
        if (!doorPositions.count({ topCentX, topRoom.Bottom() + 1 }))
        {
            Engine::Get().SpawnActor<Door>(Vector2(topCentX, topRoom.Bottom() + 1));
            doorPositions.insert({ topCentX, topRoom.Bottom() + 1 });
        }

        // 아래쪽 방 세로선.
        for (int y = bottomRoom.Top() - 1; y >= midY; --y)
        {
            if (!spawnedPositions.count({ bottomCentX, y }))
            {
                Engine::Get().SpawnActor<Corridor>(Vector2(bottomCentX, y));
                spawnedPositions.insert({ bottomCentX, y });
            }
        }
        // 문 생성.
        if (!doorPositions.count({ bottomCentX, bottomRoom.Top() - 1 }))
        {
            Engine::Get().SpawnActor<Door>(Vector2(bottomCentX, bottomRoom.Top() - 1));
            doorPositions.insert({ bottomCentX, bottomRoom.Top() - 1 });
        }

        int leftCentX = leftRoom.Center().x & 1 ? leftRoom.Center().x - 1 : leftRoom.Center().x;
        int rightCentX = rightRoom.Center().x & 1 ? rightRoom.Center().x - 1 : rightRoom.Center().x;

        // 가로선.
        for (int x = leftCentX; x <= rightCentX; x += 2)
        {
            if (!spawnedPositions.count({ x, midY }))
            {
                Engine::Get().SpawnActor<Corridor>(Vector2(x, midY));
                spawnedPositions.insert({ x, midY });
            }
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
                        if (!spawnedPositions.count({ x, leftRoom.Center().y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(x, leftRoom.Center().y));
                            spawnedPositions.insert({ x, leftRoom.Center().y });
                        }
                    }
                    else
                    {
                        if (!spawnedPositions.count({ x, midY }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(x, midY));
                            spawnedPositions.insert({ x, midY });
                        }
                    }
                }
                // 문 생성.
                if (!doorPositions.count({ leftRoom.Right() + 2, leftRoom.Center().y }))
                {
                    Engine::Get().SpawnActor<Door>(Vector2(leftRoom.Right() + 2, leftRoom.Center().y));
                    doorPositions.insert({ leftRoom.Right() + 2, leftRoom.Center().y });
                }

                for (int y = rightRoom.Top() - 1; y >= leftRoom.Center().y; --y)
                {
                    if (y > midY)
                    {
                        if (!spawnedPositions.count({ rightCentX, y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(rightCentX, y));
                            spawnedPositions.insert({ rightCentX, y });
                        }
                    }
                    else
                    {
                        if (!spawnedPositions.count({ midX, y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(midX, y));
                            spawnedPositions.insert({ midX, y });
                        }
                    }
                }
                // 문 생성.
                if (!doorPositions.count({ rightCentX, rightRoom.Top() - 1 }))
                {
                    Engine::Get().SpawnActor<Door>(Vector2(rightCentX, rightRoom.Top() - 1));
                    doorPositions.insert({ rightCentX, rightRoom.Top() - 1 });
                }
            }
            // 아래쪽->오른쪽.
            else
            {
                int midY = (leftRoom.Bottom() + rightRoom.Top()) / 2;

                for (int y = leftRoom.Bottom() + 1; y <= rightRoom.Center().y; ++y)
                {
                    if (y < midY)
                    {
                        if (!spawnedPositions.count({ leftCentX, y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(leftCentX, y));
                            spawnedPositions.insert({ leftCentX, y });
                        }
                    }
                    else
                    {
                        if (!spawnedPositions.count({ midX, y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(midX, y));
                            spawnedPositions.insert({ midX, y });
                        }
                    }
                }
                // 문 생성.
                if (!doorPositions.count({ leftCentX, leftRoom.Bottom() + 1 }))
                {
                    Engine::Get().SpawnActor<Door>(Vector2(leftCentX, leftRoom.Bottom() + 1));
                    doorPositions.insert({ leftCentX, leftRoom.Bottom() + 1 });
                }

                for (int x = rightRoom.Left() - 2; x >= leftCentX; x -= 2)
                {
                    if (x > midX)
                    {
                        if (!spawnedPositions.count({ x, rightRoom.Center().y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(x, rightRoom.Center().y));
                            spawnedPositions.insert({ x, rightRoom.Center().y });
                        }
                    }
                    else
                    {
                        if (!spawnedPositions.count({ x, midY }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(x, midY));
                            spawnedPositions.insert({ x, midY });
                        }
                    }
                }
                // 문 생성.
                if (!doorPositions.count({ rightRoom.Left() - 2, rightRoom.Center().y }))
                {
                    Engine::Get().SpawnActor<Door>(Vector2(rightRoom.Left() - 2, rightRoom.Center().y));
                    doorPositions.insert({ rightRoom.Left() - 2, rightRoom.Center().y });
                }
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
                        if (!spawnedPositions.count({ x, leftRoom.Center().y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(x, leftRoom.Center().y));
                            spawnedPositions.insert({ x, leftRoom.Center().y });
                        }
                    }
                    else
                    {
                        if (!spawnedPositions.count({ x, midY }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(x, midY));
                            spawnedPositions.insert({ x, midY });
                        }
                    }
                }
                // 문 생성.
                if (!doorPositions.count({ leftRoom.Right() + 2, leftRoom.Center().y }))
                {
                    Engine::Get().SpawnActor<Door>(Vector2(leftRoom.Right() + 2, leftRoom.Center().y));
                    doorPositions.insert({ leftRoom.Right() + 2, leftRoom.Center().y });
                }

                for (int y = rightRoom.Bottom() + 1; y <= leftRoom.Center().y; ++y)
                {
                    if (y < midY)
                    {
                        if (!spawnedPositions.count({ rightCentX, y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(rightCentX, y));
                            spawnedPositions.insert({ rightCentX, y });
                        }
                    }
                    else
                    {
                        if (!spawnedPositions.count({ midX, y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(midX, y));
                            spawnedPositions.insert({ midX, y });
                        }
                    }
                }
                // 문 생성.
                if (!doorPositions.count({ rightCentX, rightRoom.Bottom() + 1 }))
                {
                    Engine::Get().SpawnActor<Door>(Vector2(rightCentX, rightRoom.Bottom() + 1));
                    doorPositions.insert({ rightCentX, rightRoom.Bottom() + 1 });
                }
            }
            // 위쪽->오른쪽.
            else
            {
                int midY = (leftRoom.Top() + rightRoom.Bottom()) / 2;

                for (int y = leftRoom.Top() - 1; y >= rightRoom.Center().y; --y)
                {
                    if (y > midY)
                    {
                        if (!spawnedPositions.count({ leftCentX, y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(leftCentX, y));
                            spawnedPositions.insert({ leftCentX, y });
                        }
                    }
                    else
                    {
                        if (!spawnedPositions.count({ midX, y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(midX, y));
                            spawnedPositions.insert({ midX, y });
                        }
                    }
                }
                // 문 생성.
                if (!doorPositions.count({ leftCentX, leftRoom.Top() - 1 }))
                {
                    Engine::Get().SpawnActor<Door>(Vector2(leftCentX, leftRoom.Top() - 1));
                    doorPositions.insert({ leftCentX, leftRoom.Top() - 1 });
                }

                for (int x = rightRoom.Left() - 2; x >= leftCentX; x -= 2)
                {
                    if (x > midX)
                    {
                        if (!spawnedPositions.count({ x, rightRoom.Center().y }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(x, rightRoom.Center().y));
                            spawnedPositions.insert({ x, rightRoom.Center().y });
                        }
                    }
                    else
                    {
                        if (!spawnedPositions.count({ x, midY }))
                        {
                            Engine::Get().SpawnActor<Corridor>(Vector2(x, midY));
                            spawnedPositions.insert({ x, midY });
                        }
                    }
                }
                // 문 생성.
                if (!doorPositions.count({ rightRoom.Left() - 2, rightRoom.Center().y }))
                {
                    Engine::Get().SpawnActor<Door>(Vector2(rightRoom.Left() - 2, rightRoom.Center().y));
                    doorPositions.insert({ rightRoom.Left() - 2, rightRoom.Center().y });
                }
            }
        }



        //int x1 = room1.Center().x;
        //int y1 = room1.Center().y;
        //int x2 = room2.Center().x;
        //int y2 = room2.Center().y;

        //// x 축은 짝수 칸에만 생성할 수 있게 수정.
        //x1 = x1 & 1 ? x1 - 1 : x1;
        //x2 = x2 & 1 ? x2 - 1 : x2;

        //int minX = x1 < x2 ? x1 : x2;
        //int maxX = x1 >= x2 ? x1 : x2;
        //int minY = y1 < y2 ? y1 : y2;
        //int maxY = y1 >= y2 ? y1 : y2;

        //int midX = (minX + maxX) / 2;
        //int midY = (minY + maxY) / 2;

        //int random = Random(0, 1);

        //if (true)
        //{
        //    for (int x = minX; x <= maxX; x += 2)
        //    {
        //        if (x < midX)
        //        {
        //            if (!spawnedPositions.count({ x, y1 }))
        //            {
        //                Engine::Get().SpawnActor<Corridor>(Vector2(x, y1));
        //                spawnedPositions.insert({ x, y1 });
        //            }
        //        }
        //        else
        //        {
        //            if (!spawnedPositions.count({ x, midY }))
        //            {
        //                Engine::Get().SpawnActor<Corridor>(Vector2(x, midY));
        //                spawnedPositions.insert({ x, midY });
        //            }
        //        }
        //    }

        //    for (int y = minY; y <= maxY; ++y)
        //    {
        //        if (y < midY)
        //        {
        //            if (!spawnedPositions.count({ x2, y }))
        //            {
        //                Engine::Get().SpawnActor<Corridor>(Vector2(x2, y));
        //                spawnedPositions.insert({ x2, y });
        //            }
        //        }
        //        else
        //        {
        //            if (!spawnedPositions.count({ midX, y }))
        //            {
        //                Engine::Get().SpawnActor<Corridor>(Vector2(midX, y));
        //                spawnedPositions.insert({ midX, y });
        //            }
        //        }
        //    }
        //}
        //else
        //{
        //    for (int x = minX; x <= maxX; x += 2)
        //    {
        //        if (!spawnedPositions.count({ x, y2 }))
        //        {
        //            Engine::Get().SpawnActor<Corridor>(Vector2(x, y2));
        //            spawnedPositions.insert({ x, y2 });
        //        }
        //    }

        //    for (int y = minY; y <= maxY; ++y)
        //    {
        //        if (!spawnedPositions.count({ x1, y }))
        //        {
        //            Engine::Get().SpawnActor<Corridor>(Vector2(x1, y));
        //            spawnedPositions.insert({ x1, y });
        //        }
        //    }
        //}
    }
}

void Map::SpawnWalls()
{
    // 벽 생성.
    for (const auto& room : rooms)
    {
        for (int y = room.Top() - 1; y <= room.Bottom() + 1; ++y)
        {
            if (y != room.Top() - 1 && y != room.Bottom() + 1)
            {
                if (!spawnedPositions.count({ room.Left() - 2, y }))
                {
                    Engine::Get().SpawnActor<Wall>(Vector2(room.Left() - 2, y));
                    spawnedPositions.insert({ room.Left() - 2, y });
                }

                if (!spawnedPositions.count({ room.Right() + 2, y }))
                {
                    Engine::Get().SpawnActor<Wall>(Vector2(room.Right() + 2, y));
                    spawnedPositions.insert({ room.Right() + 2, y });
                }

                continue;
            }
            for (int x = room.Left() - 2; x <= room.Right() + 2; x += 2)
            {
                if (!spawnedPositions.count({ x, y }))
                {
                    Engine::Get().SpawnActor<Wall>(Vector2(x, y));
                    spawnedPositions.insert({ x, y });
                }
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

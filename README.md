# Console Rythme Rogue

로그, 크립트 오브 더 네크로댄서를 모방한 간단한 콘솔 게임 프로젝트

### 진행 기간
2025.01.16 ~ 2025.01.23

### 영상
[![Console Rythme Rogue](https://github.com/user-attachments/assets/033f25a5-11b7-419b-91f4-c384a99c4935)](https://youtu.be/pO-EOWnjosE)


### 기술 스택
- C++/Windows 콘솔, UTF-16 문자열 출력, 더블 버퍼링, 랜덤 맵 생성, 바운딩 박스 충돌 처리, 시야 시스템
- 콘솔 환경에서도 시각적인 표현을 최대한 끌어내기 위해 UTF-16 문자열 출력을 활용했고, 로그라이크 장르의 특성을 살리기 위해 랜덤 맵 생성, 바운딩 박스 충돌 처리, 시야 시스템을 구현했습니다.

### 프로젝트 진행 과정
1. 엔진 구조 및 렌더링 구현
- 콘솔 환경에서 게임 루프를 구현해서 실시간 입력을 받고 반응하도록 구현했습니다.
- 엔진 클래스에서 레벨 클래스를 하나 갖도록 했고 레벨 클래스에서 액터들의 리스트를 가지고 있도록 했습니다. 매 프레임 레벨에서 액터를 순회하면서 Tick 함수를 호출해 각각 액터별로 자신의 행동을 처리하도록 했습니다.
- 더블 버퍼링을 사용해서 화면이 업데이트될 때 깜빡이는 현상을 해결했습니다.
- UTF-16 문자열을 사용해 더 다양한 문자를 표시할 수 있도록 확장시켰습니다.
2. 액터 구조 및 플레이어 이동 구현
- 위치와 형태를 갖는 액터(Actor) 클래스 만들었고 액터 클래스를 상속받아 리듬에 맞게 움직일 수 있는 RythmeActor 클래스를 만들었습니다.
- RythmeActor을 상속받아 적과 플레이어 클래스를 만들었습니다.
- 플레이어가 리듬에 맞게 입력을 한 경우 이동이 성공하도록 구현했습니다.
3. 충돌 처리, 적 구현
- 간단한 바운딩 박스를 이용해 액터 문자열의 크기만큼 충돌을 처리하고 충돌된 두 액터에게 충돌됐다고 알려주는 함수를 호출하도록 구현했습니다.
- 정적인 물체인지 움직일 수 있는 물체인지 구분할 수 있도록 플래그를 만들고 정적인 물체들끼리는 충돌 처리를 하지 않도록 했습니다.
- 열거형을 사용해서 액터끼리 충돌 가능 여부를 정의했습니다.
- 적을 구현해서 플레이어와 적이 충돌할 시에 움직이는 방향과 같은 경우 서로에게 데미지를 입히도록 구현했습니다.
4. 맵 랜덤 생성 구현
- 방을 랜덤으로 생성을 시도하도록 하고 겹치는 경우 다른 위치를 찾는 방식으로 구현했습니다.
- 요청한 개수만큼 시도하고 무한 루프에 빠지지 않도록 최대 시도 횟수를 설정했습니다.
- MST 알고리즘을 최소한의 복도를 사용해서 방을 연결하도록 했고 맨해튼 거리로 계산해 가까운 방들과 연결하도록 했습니다.
- 시야를 구현해 플레이어가 복도에 있을 때는 한 칸 앞을 볼 수 있도록 했고 방과 플레이어가 오버랩될 때 들어갈 방의 시야가 보이도록 했습니다. 한 번 밝혀진 시야는 계속 유지됩니다.
- 문을 구현했고 플레이어가 문과 충돌한 경우 문이 사라지도록 구현했습니다.
- 계단을 구현해서 계단에 오버랩된 경우 다음 층으로 이동하도록 구현했습니다. 모든 생성된 액터를 삭제한 후 다음 층을 랜덤으로 생성하도록 했습니다.
5. 게임 시퀀스 구현
- 게임에 들어가면 랜덤으로 맵을 생성하도록 했고 각 방에 적을 랜덤으로 생성하도록 했습니다.
- 체력과 UI를 구현했습니다.
- 죽은 경우 게임 오버 문구가 뜨도록 했습니다.
- 계단을 올라가 설정한 층 수에 도달하면 게임 클리어 문구가 뜨도록 했습니다.

### 문제 해결 과정 
- UTF-8 사용 시 가변 길이 문제
  - UTF-8은 가변 길이 인코딩이기 때문에 길이를 일일이 처리하는 번거로움이 있었습니다.
  - UTF-16도 가변 길이 인코딩이긴 하나 2바이트 아니면 두 개의 2바이트 서로게이트 페어로 이루어져 있기 때문에 새로운 구조체를 만들어 4바이트를 고정적으로 사용하도록 했고 2바이트인지 4바이트인지 표시하는 boolean 변수를 가지도록 구현했습니다.
  - 공간적으로는 손해가 있으나 가변 길이 인코딩인 경우 렌더링 시에 화면 크기만큼 제대로 렌더링하기 위해서 계산 복잡도가 너무 높아 편의를 위해 공간을 더 사용하도록 구현해 해결했습니다.
- 전각/반각 문자 문제
  - 문자는 화면에서 다양한 크기를 가지기 때문에 화면에서 위치가 어긋나는 문제가 있었습니다.
  - 사용하는 문자들에 대해 전각 문자인 경우 처리할 수 있도록 함수를 만들었고 전각 문자인 경우 화면에서 두 칸을 차지하도록 해서 화면이 어긋나는 문제를 수정했습니다.
- 중복 충돌 문제
  - 액터가 충돌하는 경우 간헐적으로 충돌 처리가 두 번 발생하는 문제가 발생했습니다.
  - 충돌 처리 시 위치를 충돌한 두 물체 중 한 물체의 위치를 되돌려야 하는데 위치를 되돌리는 방법이 잘못되어 중복 충돌 문제가 발생하는 것을 확인했습니다.
  - 다양한 경우를 고려해보고 직접 디버그를 하면서 문제 상황을 재현할 수 있도록 했습니다.
  - 액터 간 충돌 발생 시, 두 액터가 모두 움직였는지 확인하고, 둘 다 이동했다면 한 액터의 위치를 되돌린 후, 모든 액터에 대해 충돌을 다시 검사하여 문제를 해결하였습니다.
- 맵 생성 시 계단이 방에 겹치는 문제
  - 맵 생성 시, 방의 중간에서 다른 방의 중간 위치까지 직선으로 연결할 때 계단이 다른 방 위에 겹쳐서 생성되는 문제가 발생했습니다. 이를 해결하기 위해 두 가지 원인을 확인했습니다.
  - 첫 번째 원인은 맨해튼 거리를 계산하는 식에 있었습니다. 콘솔 환경에서는 가로와 세로의 길이가 다르기 때문에 가로 2칸을 세로 1칸과 동일하게 취급하고 개발하고 있었습니다. 이 부분을 고려하지 않아 가까운 방이 아닌 먼 방이 선택될 수 있었고, 복도가 방 위에 생성될 가능성이 있었습니다. 이를 해결하기 위해 가로 길이를 계산하는 식을 수정했고, 많은 문제가 해결되었습니다.
  - 하지만 여전히 문제가 발생했고, 저는 방과 방을 연결할 때 복도의 연결 방식을 상황에 따라 다르게 처리했습니다. 방과 방이 가로 또는 세로로 평행한 경우, 그리고 대각선으로 엇갈리는 경우 두 가지로 나누어 복도의 꺾이는 횟수를 달리 구현하여 문제를 해결했습니다.

### 개선할 사항
- 물리 연산을 현재와 같은 스레드에서 처리하고 있지만, 현재 프로젝트에서는 문제가 없으나, 더 큰 규모의 게임에서는 물리 연산을 비동기적으로 처리해야 할 필요성이 있을 것 같습니다. 또한, 충돌 처리 시 쿼드 트리를 사용하여 최적화를 시도할 수 있을 것입니다.
- 시스템 구현에 중점을 두었지만, 게임에 필요한 다른 시스템들도 함께 구현했으면 더 좋았을 것 같습니다.
- 현재 레벨에서 액터의 리스트를 로우 포인터로 관리하고 있으며, 삭제 처리가 이루어진 후 다른 곳에서 해당 액터를 참조할 경우 삭제 여부를 확인하는 방법이 없습니다. 이 문제를 해결하기 위해, 현재는 액터가 삭제되기 전에 isActive를 false로 설정해두고 있지만, 스마트 포인터를 사용하면 액터의 삭제 여부를 적절하게 처리할 수 있을 것입니다.

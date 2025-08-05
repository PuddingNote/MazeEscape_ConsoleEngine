#include "Enemy.h"
#include "Engine.h"
#include "Input.h"
#include "Level/Level.h"
#include "Actor/Actor.h"
#include "Utils/Utils.h"

#include "Wall.h"
#include "Target.h"
#include "Path.h"

#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iostream>

Enemy::Enemy(const Vector2& position) : Actor('E', Color::Blue, position)
{
    SetSortingOrder(2);
}

Enemy::~Enemy()
{

}

void Enemy::BeginPlay()
{
    super::BeginPlay();

    // 모든 Actor를 순회해서 Target 찾기
    Vector2 targetPosition = Vector2::Zero;
    if (GetOwner())
    {
        for (Actor* const actor : GetOwner()->GetActors())
        {
            if (actor->As<Target>())
            {
                targetPosition = actor->Position();
                break;
            }
        }
    }

    // Target을 찾았으면 해당 Position으로 경로 탐색
    if (targetPosition.x != 0 || targetPosition.y != 0)
    {
        FindPathToTarget(targetPosition);
    }
}

void Enemy::Tick(float deltaTime)
{
    super::Tick(deltaTime);

    if (isMovementStopped || !hasPath || pathToTarget.empty())
    {
        return;
    }

    // 이동 타이머 업데이트
    moveTimer += deltaTime;

    if (moveTimer >= 0.8f - moveSpeed)
    {
        moveTimer = 0.0f;

        // 다음 경로 지점으로 이동
        if (currentPathIndex < pathToTarget.size())
        {
            Vector2 nextPosition = pathToTarget[currentPathIndex];
            SetPosition(nextPosition);
            currentPathIndex++;
        }
    }
}

// A* 알고리즘으로 최단 경로 탐색
void Enemy::FindPathToTarget(const Vector2& targetPosition)
{
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<int, bool> closedSet;    // 방문 완료된 노드
    std::unordered_map<int, Vector2> cameFrom;  // 경로 추적
    std::unordered_map<int, int> gScore;        // 시작점부터의 거리
    std::unordered_map<int, int> fScore;        // 휴리스틱 + gScore

    Vector2 start = Position();
    int startHash = start.x + start.y * 1000;

    // 초기화
    openSet.push(Node(start, 0, Heuristic(start, targetPosition)));
    gScore[startHash] = 0;
    fScore[startHash] = Heuristic(start, targetPosition);

    while (!openSet.empty())
    {
        Node current = openSet.top();
        openSet.pop();

        int currentHash = current.pos.x + current.pos.y * 1000;

        // 이미 방문한 노드면 스킵
        if (closedSet.find(currentHash) != closedSet.end())
        {
            continue;
        }

        closedSet[currentHash] = true;

        // 타겟에 도달했으면 경로 재구성
        if (current.pos == targetPosition)
        {
            // 경로 재구성
            std::vector<Vector2> path;
            Vector2 currentPos = targetPosition;

            while (currentPos.x != start.x || currentPos.y != start.y)
            {
                path.push_back(currentPos);
                int hash = currentPos.x + currentPos.y * 1000;
                currentPos = cameFrom[hash];
            }
            path.push_back(start);

            // 경로를 시작점에서 타겟까지 순서로 뒤집기
            std::reverse(path.begin(), path.end());

            SetPath(path);
            return;
        }

        // 4방향 탐색
        Vector2 directions[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
        for (int i = 0; i < 4; ++i)
        {
            Vector2 next = current.pos + directions[i];
            int nextHash = next.x + next.y * 1000;

            // 이동 가능한 위치인지 확인
            if (!CanMoveTo(next))
            {
                continue;
            }

            // 이미 방문한 노드면 스킵
            if (closedSet.find(nextHash) != closedSet.end())
            {
                continue;
            }

            int tentativeGScore = current.gScore + 1;  // 한 칸 이동 = 거리 1

            // 더 좋은 경로를 찾았거나 처음 방문하는 노드
            if (gScore.find(nextHash) == gScore.end() || tentativeGScore < gScore[nextHash])
            {
                cameFrom[nextHash] = current.pos;
                gScore[nextHash] = tentativeGScore;
                fScore[nextHash] = tentativeGScore + Heuristic(next, targetPosition);

                openSet.push(Node(next, tentativeGScore, fScore[nextHash]));
            }
        }
    }

    // 경로를 찾지 못한 경우
    hasPath = false;
}

int Enemy::Heuristic(const Vector2& from, const Vector2& to) const
{
    // 맨해튼 거리 (Manhattan Distance)
    return abs(from.x - to.x) + abs(from.y - to.y);
}

bool Enemy::CanMoveTo(const Vector2& position)
{
    if (!GetOwner())
    {
        return false;
    }

    // Todo: 범위 체크인데 임시방편으로 수치를 넣음
    if (position.x < 0 || position.x >= 60 || position.y < 0 || position.y >= 40)
    {
        return false;
    }

    // 해당 위치의 Actor 확인
    for (Actor* const actor : GetOwner()->GetActors())
    {
        if (actor->Position().x == position.x && actor->Position().y == position.y) // 직접 비교
        {
            // Wall이면 이동 불가
            if (actor->As<Wall>())
            {
                return false;
            }

            // Ground, Target, Player는 이동 가능
            return true;
        }
    }

    // Actor가 없는 경우 (Ground로 간주)
    return true;
}

// 경로 설정 함수
void Enemy::SetPath(const std::vector<Vector2>& path)
{
    pathToTarget = path;
    currentPathIndex = 0;
    moveTimer = 0.0f;
    hasPath = true;
}

void Enemy::CreatePathActors()
{
    // 기존 Path 액터들 제거
    ClearPathActors();

    // 현재 위치부터 Target까지의 남은 경로만 Path 액터 생성
    for (size_t i = currentPathIndex; i < pathToTarget.size() - 1; ++i)
    {
        Path* pathActor = new Path(pathToTarget[i]);
        pathActors.push_back(pathActor);

        // 레벨에 Path 액터 추가
        if (GetOwner())
        {
            GetOwner()->AddActor(pathActor);
        }
    }
}

void Enemy::ClearPathActors()
{
    // Path 액터들 제거
    for (Path* pathActor : pathActors)
    {
        if (pathActor)
        {
            SafeDelete(pathActor);
        }
    }
    pathActors.clear();
}

bool Enemy::RemoveNextPath()
{
    if (pathActors.empty())
    {
        return false;
    }

    // 첫 번째 Path Actor 제거
    Path* pathActor = pathActors[0];
    if (pathActor && GetOwner())
    {
        // 해당 위치를 공백으로 지우기
        Vector2 pos = pathActor->Position();
        Utils::SetConsolePosition({ (short)pos.x, (short)pos.y });
        std::cout << ' ';

        // Level에서 해당 Actor 제거 (SafeDelete는 Level의 RemoveActor에서 처리)
        GetOwner()->RemoveActor(pathActor);
    }

    // 벡터에서도 제거
    pathActors.erase(pathActors.begin());

    // 남은 Path가 있는지 확인
    return !pathActors.empty();
}

void Enemy::StopMovement()
{
    isMovementStopped = true;
}

void Enemy::SetMoveSpeed(float speed)
{
    moveSpeed = speed;
}
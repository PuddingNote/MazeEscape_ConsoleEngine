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

    // ��� Actor�� ��ȸ�ؼ� Target ã��
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

    // Target�� ã������ �ش� Position���� ��� Ž��
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

    // �̵� Ÿ�̸� ������Ʈ
    moveTimer += deltaTime;

    if (moveTimer >= 0.8f - moveSpeed)
    {
        moveTimer = 0.0f;

        // ���� ��� �������� �̵�
        if (currentPathIndex < pathToTarget.size())
        {
            Vector2 nextPosition = pathToTarget[currentPathIndex];
            SetPosition(nextPosition);
            currentPathIndex++;
        }
    }
}

// A* �˰������� �ִ� ��� Ž��
void Enemy::FindPathToTarget(const Vector2& targetPosition)
{
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
    std::unordered_map<int, bool> closedSet;    // �湮 �Ϸ�� ���
    std::unordered_map<int, Vector2> cameFrom;  // ��� ����
    std::unordered_map<int, int> gScore;        // ������������ �Ÿ�
    std::unordered_map<int, int> fScore;        // �޸���ƽ + gScore

    Vector2 start = Position();
    int startHash = start.x + start.y * 1000;

    // �ʱ�ȭ
    openSet.push(Node(start, 0, Heuristic(start, targetPosition)));
    gScore[startHash] = 0;
    fScore[startHash] = Heuristic(start, targetPosition);

    while (!openSet.empty())
    {
        Node current = openSet.top();
        openSet.pop();

        int currentHash = current.pos.x + current.pos.y * 1000;

        // �̹� �湮�� ���� ��ŵ
        if (closedSet.find(currentHash) != closedSet.end())
        {
            continue;
        }

        closedSet[currentHash] = true;

        // Ÿ�ٿ� ���������� ��� �籸��
        if (current.pos == targetPosition)
        {
            // ��� �籸��
            std::vector<Vector2> path;
            Vector2 currentPos = targetPosition;

            while (currentPos.x != start.x || currentPos.y != start.y)
            {
                path.push_back(currentPos);
                int hash = currentPos.x + currentPos.y * 1000;
                currentPos = cameFrom[hash];
            }
            path.push_back(start);

            // ��θ� ���������� Ÿ�ٱ��� ������ ������
            std::reverse(path.begin(), path.end());

            SetPath(path);
            return;
        }

        // 4���� Ž��
        Vector2 directions[4] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };
        for (int i = 0; i < 4; ++i)
        {
            Vector2 next = current.pos + directions[i];
            int nextHash = next.x + next.y * 1000;

            // �̵� ������ ��ġ���� Ȯ��
            if (!CanMoveTo(next))
            {
                continue;
            }

            // �̹� �湮�� ���� ��ŵ
            if (closedSet.find(nextHash) != closedSet.end())
            {
                continue;
            }

            int tentativeGScore = current.gScore + 1;  // �� ĭ �̵� = �Ÿ� 1

            // �� ���� ��θ� ã�Ұų� ó�� �湮�ϴ� ���
            if (gScore.find(nextHash) == gScore.end() || tentativeGScore < gScore[nextHash])
            {
                cameFrom[nextHash] = current.pos;
                gScore[nextHash] = tentativeGScore;
                fScore[nextHash] = tentativeGScore + Heuristic(next, targetPosition);

                openSet.push(Node(next, tentativeGScore, fScore[nextHash]));
            }
        }
    }

    // ��θ� ã�� ���� ���
    hasPath = false;
}

int Enemy::Heuristic(const Vector2& from, const Vector2& to) const
{
    // ����ư �Ÿ� (Manhattan Distance)
    return abs(from.x - to.x) + abs(from.y - to.y);
}

bool Enemy::CanMoveTo(const Vector2& position)
{
    if (!GetOwner())
    {
        return false;
    }

    // Todo: ���� üũ�ε� �ӽù������� ��ġ�� ����
    if (position.x < 0 || position.x >= 60 || position.y < 0 || position.y >= 40)
    {
        return false;
    }

    // �ش� ��ġ�� Actor Ȯ��
    for (Actor* const actor : GetOwner()->GetActors())
    {
        if (actor->Position().x == position.x && actor->Position().y == position.y) // ���� ��
        {
            // Wall�̸� �̵� �Ұ�
            if (actor->As<Wall>())
            {
                return false;
            }

            // Ground, Target, Player�� �̵� ����
            return true;
        }
    }

    // Actor�� ���� ��� (Ground�� ����)
    return true;
}

// ��� ���� �Լ�
void Enemy::SetPath(const std::vector<Vector2>& path)
{
    pathToTarget = path;
    currentPathIndex = 0;
    moveTimer = 0.0f;
    hasPath = true;
}

void Enemy::CreatePathActors()
{
    // ���� Path ���͵� ����
    ClearPathActors();

    // ���� ��ġ���� Target������ ���� ��θ� Path ���� ����
    for (size_t i = currentPathIndex; i < pathToTarget.size() - 1; ++i)
    {
        Path* pathActor = new Path(pathToTarget[i]);
        pathActors.push_back(pathActor);

        // ������ Path ���� �߰�
        if (GetOwner())
        {
            GetOwner()->AddActor(pathActor);
        }
    }
}

void Enemy::ClearPathActors()
{
    // Path ���͵� ����
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

    // ù ��° Path Actor ����
    Path* pathActor = pathActors[0];
    if (pathActor && GetOwner())
    {
        // �ش� ��ġ�� �������� �����
        Vector2 pos = pathActor->Position();
        Utils::SetConsolePosition({ (short)pos.x, (short)pos.y });
        std::cout << ' ';

        // Level���� �ش� Actor ���� (SafeDelete�� Level�� RemoveActor���� ó��)
        GetOwner()->RemoveActor(pathActor);
    }

    // ���Ϳ����� ����
    pathActors.erase(pathActors.begin());

    // ���� Path�� �ִ��� Ȯ��
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
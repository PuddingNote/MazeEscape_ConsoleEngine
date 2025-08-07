#pragma once

#include "Actor/Actor.h"

#include <vector>

class Enemy : public Actor
{
    // BFS ��� ����ü
    struct NodeBFS
    {
        Vector2 pos;
        Vector2 parent;  // �θ� ��� ��ġ (��� �籸����)

        NodeBFS(const Vector2& p, const Vector2& par) : pos(p), parent(par) {}
    };

    // A* ��� ����ü
    struct Node
    {
        Vector2 pos;
        int gScore;     // ������������ �Ÿ�
        int fScore;     // �޸���ƽ + gScore

        Node(const Vector2& p, int g, int f) : pos(p), gScore(g), fScore(f) {}

        // �켱���� ť�� ���� �� ������ (fScore�� �������� �켱)
        bool operator>(const Node& other) const
        {
            return fScore > other.fScore;
        }
    };

    RTTI_DECLARATIONS(Enemy, Actor)

public:
    Enemy(const Vector2& position);

    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    void SetMazeSize(int width, int height);    // �̷� ���� üũ�� ���� �̷� ������ ���� �Լ�

    void FindPathToTarget(const Vector2& targetPosition);	    // ��� Ž�� (A*)
    void FindPathToTargetBFS(const Vector2& targetPosition);    // ��� Ž�� (BFS)
    bool CanMoveTo(const Vector2& position);				    // �̵� ���� Ȯ��
    void SetPath(const std::vector<Vector2>& path);			    // ��� ����

    // Path ���� ����
    void CreatePathActors();        // ��� Path ���� ����
    void ClearPathActors();	        // ���� Path ���� ����
    bool RemoveNextPath();          // ���� Path ���� (��ȯ��: ���� Path�� �ִ���)

    // ���� Ŭ���� ����
    void StopMovement();            // �̵� ����
    void SetMoveSpeed(float speed); // �̵��ӵ� ����

    // Ž�� ���� �м��� �Լ���
    int GetLastSearchCount() const { return lastSearchCount; }  // ������ Ž�� Ƚ�� ��ȯ
    void ResetSearchCount() { lastSearchCount = 0; }            // Ž�� Ƚ�� �ʱ�ȭ
    int GetBFSCount() const { return bfsCount; }                // BFS ī��Ʈ getter
    int GetAStarCount() const { return astarCount; }            // A* ī��Ʈ getter

private:
    int Heuristic(const Vector2& from, const Vector2& to) const;  // �޸���ƽ �Լ� (����ư �Ÿ�)

private:
    int mazeWidth = 25;
    int mazeHeight = 15;

    // ���� ���۽� ��� ����
    std::vector<Vector2> pathToTarget;	    // Ÿ�ٱ����� ���
    int currentPathIndex = 0;			    // ���� ��� �ε���
    float moveSpeed = 0.0f;	    		    // �̵� �ӵ�
    float moveTimer = 0.0f;				    // �̵� Ÿ�̸�
    bool hasPath = false;				    // ��� Ž�� �Ϸ� ����

    // ���� Ŭ���� ���� ��� ����
    std::vector<class Path*> pathActors;    // ��θ� ǥ���ϴ� Path ���͵�
    bool isMovementStopped = false;         // �̵� ���� ����

    // Ž�� ���� �м��� ������
    mutable int lastSearchCount = 0;
    int astarCount = 0;
    int bfsCount = 0;
};
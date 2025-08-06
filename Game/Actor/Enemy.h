#pragma once

#include "Actor/Actor.h"

#include <vector>

class Enemy : public Actor
{
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

    void FindPathToTarget(const Vector2& targetPosition);	// ��� Ž��
    bool CanMoveTo(const Vector2& position);				// �̵� ���� Ȯ��
    void SetPath(const std::vector<Vector2>& path);			// ��� ����

    // Path ���� ����
    void CreatePathActors();    // ��� Path ���� ����
    void ClearPathActors();	    // ���� Path ���� ����
    bool RemoveNextPath();      // ���� Path ���� (��ȯ��: ���� Path�� �ִ���)

    // ���� Ŭ���� ����
    void StopMovement();            // �̵� ����
    void SetMoveSpeed(float speed); // �̵��ӵ� ����

private:
    int Heuristic(const Vector2& from, const Vector2& to) const;  // �޸���ƽ �Լ� (����ư �Ÿ�)

private:
    int mazeWidth = 25;
    int mazeHeight = 15;

    // ���� ���۽� ��� ����
    std::vector<Vector2> pathToTarget;	// Ÿ�ٱ����� ���
    int currentPathIndex = 0;			// ���� ��� �ε���
    float moveSpeed = 0.0f;	    		// �̵� �ӵ�
    float moveTimer = 0.0f;				// �̵� Ÿ�̸�
    bool hasPath = false;				// ��� Ž�� �Ϸ� ����

    // ���� Ŭ���� ���� ��� ����
    std::vector<class Path*> pathActors;    // ��θ� ǥ���ϴ� Path ���͵�
    bool isMovementStopped = false;         // �̵� ���� ����
};
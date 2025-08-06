#pragma once

#include "Actor/Actor.h"

#include <vector>

class Enemy : public Actor
{
    // A* 노드 구조체
    struct Node
    {
        Vector2 pos;
        int gScore;     // 시작점부터의 거리
        int fScore;     // 휴리스틱 + gScore

        Node(const Vector2& p, int g, int f) : pos(p), gScore(g), fScore(f) {}

        // 우선순위 큐를 위한 비교 연산자 (fScore가 낮을수록 우선)
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

    void SetMazeSize(int width, int height);    // 미로 범위 체크를 위한 미로 사이즈 설정 함수

    void FindPathToTarget(const Vector2& targetPosition);	// 경로 탐색
    bool CanMoveTo(const Vector2& position);				// 이동 가능 확인
    void SetPath(const std::vector<Vector2>& path);			// 경로 설정

    // Path 액터 관리
    void CreatePathActors();    // 경로 Path 액터 생성
    void ClearPathActors();	    // 기존 Path 액터 제거
    bool RemoveNextPath();      // 다음 Path 제거 (반환값: 남은 Path가 있는지)

    // 게임 클리어 관련
    void StopMovement();            // 이동 중지
    void SetMoveSpeed(float speed); // 이동속도 설정

private:
    int Heuristic(const Vector2& from, const Vector2& to) const;  // 휴리스틱 함수 (맨해튼 거리)

private:
    int mazeWidth = 25;
    int mazeHeight = 15;

    // 게임 시작시 경로 관련
    std::vector<Vector2> pathToTarget;	// 타겟까지의 경로
    int currentPathIndex = 0;			// 현재 경로 인덱스
    float moveSpeed = 0.0f;	    		// 이동 속도
    float moveTimer = 0.0f;				// 이동 타이머
    bool hasPath = false;				// 경로 탐색 완료 여부

    // 게임 클리어 이후 경로 관련
    std::vector<class Path*> pathActors;    // 경로를 표시하는 Path 액터들
    bool isMovementStopped = false;         // 이동 중지 상태
};
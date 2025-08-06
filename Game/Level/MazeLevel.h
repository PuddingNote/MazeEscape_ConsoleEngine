#pragma once

#include "Level/Level.h"
#include "Interface/ICanPlayerMove.h"

class MazeLevel : public Level, public ICanPlayerMove
{
	// 미로 타입 enum
	enum class MazeCellType
	{
		Ground = 0,		// 길
		Wall = 1		// 벽
	};

	RTTI_DECLARATIONS(MazeLevel, Level)

public:
	MazeLevel(int width, int height);
	~MazeLevel();

	virtual void Render() override;
	virtual bool CanPlayerMove(const Vector2& playerPosition, const Vector2& targetPosition) override;	// 플레이어가 이동 가능한지 여부
	void SetNeedsStaticRerender(bool needs) { needsStaticRerender = needs; }

private:
	virtual void Tick(float deltaTime) override;

	void GenerateMaze();				// 미로 랜덤 생성 함수
	void GenerateMazeDFS(int x, int y);	// DFS 알고리즘으로 미로 생성 함수
	
	void InitializeActors();			// 액터 생성 함수
	void InitialRender();				// 초기 렌더링 (미로, 출구, 플레이어)
	void RegenerateMaze();				// 미로 재생성 함수

	bool IsValidCell(int x, int y);		// 유효한 셀인지 확인
	void EnsureUpDownPaths();			// 최소 2갈래 이상 경로 보장 (상, 하)
	bool HasValidPath(int startX, int startY, int targetX, int targetY); // 경로 존재 확인

	// 경로(Path) 관련
	void StartPathVisualization();					// 경로 시각화 시작
	void UpdatePathVisualization(float deltaTime);	// 경로 업데이트

	void CheckStageClear();				// 스테이지 클리어 확인 함수

private:
	// Maze Data
	int MAZE_WIDTH;
	int MAZE_HEIGHT;
	int EXIT_X;
	int EXIT_Y;
	MazeCellType** mazeData;

	// Stage Clear 관련 변수들
	bool isShowingPath = false;					// 경로 표시 중인지 확인
	float pathShowTimer = 0.0f;					// Path 표시 타이머
	const float PATH_SHOW_INTERVAL = 0.2f;		// 표시된 Path가 사라지는 간격
	const float ENEMY_SPEED_INCREMENT = 0.1f;   // 스테이지당 Enemy 속도 증가량

	bool needsStaticRerender = false;  // 재렌더링 필요 여부 확인 (벽, 길, 출구)

	const int MAX_STAGE = 2;	// 최대 스테이지 레벨
	int stageLevel = 0;			// 스테이지 레벨
	bool isStageClear = false;	// 스테이지 클리어 여부 확인 변수
};
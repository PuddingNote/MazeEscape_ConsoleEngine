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
	void SetNeedsRerender(bool needs) { needsRerender = needs; }

private:
	virtual void Tick(float deltaTime) override;

	// 미로(Maze) 관련
	void InitializeMaze();				// 미로 랜덤 생성 함수
	void GenerateMazeDFS(int x, int y);	// DFS 알고리즘으로 미로 생성 함수

	void InitializeActors();			// 액터 생성 함수
	void InitializeRender();			// 초기 렌더링 (미로, 출구, 플레이어)
	void RegenerateMaze();				// 미로 재생성 함수

	bool IsValidCell(int x, int y);		// 유효한 셀인지 확인
	void EnsureUpDownPaths();			// 최소 2갈래 이상 경로 보장 (상, 하)
	bool HasValidPath(int startX, int startY, int targetX, int targetY); // 경로 존재 확인

	// 경로(Path) 관련
	void StartPathVisualization();					// 경로 시각화 시작
	void UpdatePathVisualization(float deltaTime);	// 경로 업데이트

	// 게임(Stage) 관련
	void CheckStageClear();					// 스테이지 클리어 확인 함수
	void AddTotalScore();					// 스테이지 종료시 총점에 추가
	void AddCurrentScore();					// 스테이지 점수 +1

	// 점수 관리 기능
	int GetModeLineNumber() const;			// 모드에 따른 번호 반환
	void SaveBestScore();					// 최고점수 저장 (게임 종료 시 호출)
	int LoadBestScore() const;				// 최고점수 로드
	void ShowBestScore(short x, short y);	// Best Score 표시
	void ShowFinalScore();					// 최종 점수와 Best Score 표시

private:
	// Maze Data
	int MAZE_WIDTH;
	int MAZE_HEIGHT;
	int EXIT_X;
	int EXIT_Y;
	MazeCellType** mazeData;

	// Stage Data
	const int MAX_STAGE = 5;	// 최대 스테이지 레벨 (안넣어도 되지만 Stage별 Enemy 이동속도의 최대치를 넣지 않았기때문에 넣었음)
	int stageLevel = 0;			// 스테이지 레벨
	bool isStageClear = false;	// 스테이지 클리어 여부 확인 변수
	int totalScore = 0;			// 총 점수
	int currentScore = 0;		// 현재 스테이지에서 얻은 점수

	// Stage Clear Path Data
	bool isShowingPath = false;					// 경로 표시 중인지 확인
	float pathShowTimer = 0.0f;					// Path 표시 타이머
	const float PATH_SHOW_INTERVAL = 0.1f;		// 표시된 Path가 사라지는 간격
	const float ENEMY_SPEED_INCREMENT = 0.1f;   // 스테이지당 Enemy 속도 증가량

	bool needsRerender = false;  // 재렌더링 필요 여부 확인 (벽, 길, 출구)
};
#pragma once

#include "Level/Level.h"
#include "Interface/ICanPlayerMove.h"

class MazeLevel : public Level, public ICanPlayerMove
{
	// �̷� Ÿ�� enum
	enum class MazeCellType
	{
		Ground = 0,		// ��
		Wall = 1		// ��
	};

	RTTI_DECLARATIONS(MazeLevel, Level)

public:
	MazeLevel(int width, int height);
	~MazeLevel();

	virtual void Render() override;
	virtual bool CanPlayerMove(const Vector2& playerPosition, const Vector2& targetPosition) override;	// �÷��̾ �̵� �������� ����
	void SetNeedsStaticRerender(bool needs) { needsStaticRerender = needs; }

private:
	virtual void Tick(float deltaTime) override;

	// �̷�(Maze) ����
	void GenerateMaze();				// �̷� ���� ���� �Լ�
	void GenerateMazeDFS(int x, int y);	// DFS �˰������� �̷� ���� �Լ�
	
	void InitializeActors();			// ���� ���� �Լ�
	void InitialRender();				// �ʱ� ������ (�̷�, �ⱸ, �÷��̾�)
	void RegenerateMaze();				// �̷� ����� �Լ�

	bool IsValidCell(int x, int y);		// ��ȿ�� ������ Ȯ��
	void EnsureUpDownPaths();			// �ּ� 2���� �̻� ��� ���� (��, ��)
	bool HasValidPath(int startX, int startY, int targetX, int targetY); // ��� ���� Ȯ��

	// ���(Path) ����
	void StartPathVisualization();					// ��� �ð�ȭ ����
	void UpdatePathVisualization(float deltaTime);	// ��� ������Ʈ

	// ����(Stage) ����
	void CheckStageClear();			// �������� Ŭ���� Ȯ�� �Լ�
	void AddTotalScore();			// �������� ����� ������ �߰�
	void AddCurrentScore();			// �������� ���� +1

private:
	// Maze Data
	int MAZE_WIDTH;
	int MAZE_HEIGHT;
	int EXIT_X;
	int EXIT_Y;
	MazeCellType** mazeData;

	// Stage Data
	const int MAX_STAGE = 5;	// �ִ� �������� ���� (�ȳ־ ������ Stage�� Enemy �̵��ӵ��� �ִ�ġ�� ���� �ʾұ⶧���� �־���)
	int stageLevel = 0;			// �������� ����
	bool isStageClear = false;	// �������� Ŭ���� ���� Ȯ�� ����
	int totalScore = 0;			// �� ����
	int currentScore = 0;		// ���� ������������ ���� ����

	// Stage Clear Path Data
	bool isShowingPath = false;					// ��� ǥ�� ������ Ȯ��
	float pathShowTimer = 0.0f;					// Path ǥ�� Ÿ�̸�
	const float PATH_SHOW_INTERVAL = 0.1f;		// ǥ�õ� Path�� ������� ����
	const float ENEMY_SPEED_INCREMENT = 0.1f;   // ���������� Enemy �ӵ� ������

	bool needsStaticRerender = false;  // �緻���� �ʿ� ���� Ȯ�� (��, ��, �ⱸ)
};
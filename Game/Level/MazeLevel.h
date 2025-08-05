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
	MazeLevel();

	virtual void Render() override;

	virtual bool CanPlayerMove(const Vector2& playerPosition, const Vector2& targetPosition) override;	// �÷��̾ �̵� �������� ����

	void SetNeedsStaticRerender(bool needs) { needsStaticRerender = needs; }

private:
	void GenerateMaze();				// �̷� ���� ���� �Լ�
	void GenerateMazeDFS(int x, int y);	// DFS �˰������� �̷� ���� �Լ�
	bool IsValidCell(int x, int y);		// ��ȿ�� ������ Ȯ��

	void InitializeActors();			// ���� ���� �Լ�

	void RegenerateMaze();				// �̷� ����� �Լ�

	void InitialRender();				// �ʱ� ������ (�̷�, �ⱸ, �÷��̾�)

	virtual void Tick(float deltaTime) override;
	
	void EnsureUpDownPaths();			// �ּ� 2���� �̻� ��� ���� (��, ��)
	bool HasValidPath(int startX, int startY, int targetX, int targetY); // ��� ���� Ȯ��

	// ��� ����
	void StartPathVisualization();					// ��� �ð�ȭ ����
	void UpdatePathVisualization(float deltaTime);	// ��� ������Ʈ

	void CheckStageClear();				// �������� Ŭ���� Ȯ�� �Լ�

private:
	// �̷� ũ�� ��� �׽�Ʈ�� (���� ������ �� ������ �ؽ�Ʈ���� �ҷ����°� �ִµ� ����..)
	// (15, 7), (21, 11), (27, 15), (33, 15), (39, 15), (45, 19), (51, 19), (57, 23)... (N+6, N+4)
	static const int MAZE_WIDTH = 21;
	static const int MAZE_HEIGHT = 11;

	// �ⱸ ��ġ ��� �׽�Ʈ�� (���� �ߴ� ����)
	static const int EXIT_X = MAZE_WIDTH - 2;
	static const int EXIT_Y = MAZE_HEIGHT / 2;

	// �̷� ������
	MazeCellType mazeData[MAZE_HEIGHT][MAZE_WIDTH];

	// Stage Clear ���� ������
	bool isShowingPath = false;					// ��� ǥ�� ������ Ȯ��
	float pathShowTimer = 0.0f;					// Path ǥ�� Ÿ�̸�
	const float PATH_SHOW_INTERVAL = 0.2f;		// ǥ�õ� Path�� ������� ����
	const float ENEMY_SPEED_INCREMENT = 0.1f;   // ���������� Enemy �ӵ� ������

	bool needsStaticRerender = false;  // ���� ��� �緻���� �ʿ� ����

	int stageLevel = 1;			// �������� ����
	bool isStageClear = false;	// �������� Ŭ���� ���� Ȯ�� ����
};
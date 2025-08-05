#include "MazeLevel.h"
#include "Math/Vector2.h"
#include "Utils/Utils.h"
#include "Engine.h"

#include "Actor/Actor.h"
#include "Actor/Wall.h"
#include "Actor/Ground.h"
#include "Actor/Target.h"
#include "Actor/Player.h"
#include "Actor/Enemy.h"
#include "Actor/Path.h"

#include <iostream>
#include <random>
#include <ctime>

MazeLevel::MazeLevel()
{
	GenerateMaze();
}

// �̷� ���� ���� ���� �Լ�
void MazeLevel::GenerateMaze()
{
	// ���� �õ� �ʱ�ȭ
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// �̷� ������ �ʱ�ȭ (��� ������ �ʱ�ȭ)
	for (int y = 0; y < MAZE_HEIGHT; ++y)
	{
		for (int x = 0; x < MAZE_WIDTH; ++x)
		{
			mazeData[y][x] = MazeCellType::Wall;
		}
	}

	// DFS�� �̷� ���� (�ⱸ���� �����Ͽ� �̷� ����)
	GenerateMazeDFS(EXIT_X, EXIT_Y);

	// �ּ� 2���� �̻� ��� ���� (��, ��)
	EnsureUpDownPaths();

	// Actor ���� (��, ��)
	for (int y = 0; y < MAZE_HEIGHT; ++y)
	{
		for (int x = 0; x < MAZE_WIDTH; ++x)
		{
			if (mazeData[y][x] == MazeCellType::Wall)
			{
				AddActor(new Wall(Vector2(x, y)));		// ��
			}
			else
			{
				AddActor(new Ground(Vector2(x, y)));	// ��
			}
		}
	}

	// Actor ���� (��, �÷��̾�)
	InitializeActors();

	// �ʱ� ������ (��, ��, �ⱸ)
	InitialRender();
}

// DFS�� ����� ���� �̷� ���� �Լ�
void MazeLevel::GenerateMazeDFS(int x, int y)
{
	// ���� ��ġ�� ��� ���� (�湮�� ���� ��� ����� ǥ��: �� ��ġ�� �̹� ó���Ǿ����� ǥ��)
	mazeData[y][x] = MazeCellType::Ground;

	// 4���� (��, ��, ��, ��) ���� ������ Ž�� (2ĭ�� �̵��ϴ� ����: ���� ���� ������ ��Ÿ���� ����)
	int directions[4][2] = { {0, -2}, {0, 2}, {-2, 0}, {2, 0} };

	// ������ �����ϰ� ���� (Fisher-Yates ����: �Ź� �ٸ� ������ ������ Ž���ؼ� �̷��� �پ缺 ����)
	int indices[4] = { 0, 1, 2, 3 };
	for (int i = 3; i > 0; --i)
	{
		int j = std::rand() % (i + 1);
		std::swap(indices[i], indices[j]);
	}

	// 4���� ������ ������ ������ �ϳ��� �õ�
	for (int i = 0; i < 4; ++i)
	{
		int dx = directions[indices[i]][0];
		int dy = directions[indices[i]][1];
		int newX = x + dx;
		int newY = y + dy;

		// ��ȿ�� ���̰� ���� �湮���� ���� ���
		if (IsValidCell(newX, newY) && mazeData[newY][newX] == MazeCellType::Wall)
		{
			// ���� ��ġ�� ���ο� ��ġ�� ��� ���� ��� �ٲ� ���� ����
			mazeData[y + dy / 2][x + dx / 2] = MazeCellType::Ground;

			// ���ο� ��ġ���� ��������� �� �ձ� ����
			GenerateMazeDFS(newX, newY);
		}
	}
}

// ��ȿ�� ������ Ȯ���ϴ� �Լ� (�̷� ������ �ľ��� ����: �̷� ���� ���� �ְ�, �����ڸ��� �ƴ� ���)
bool MazeLevel::IsValidCell(int x, int y)
{
	return x > 0 && x < MAZE_WIDTH - 1 && y > 0 && y < MAZE_HEIGHT - 1;
}

// �ʱ� ���� ���� �Լ� (�ⱸ, ��, �÷��̾�)
void MazeLevel::InitializeActors()
{
	// �ⱸ ���� (����: ���� �ߴ� / ���߿�: �߾� ��� or ���� �״��)
	AddActor(new Target(Vector2(EXIT_X, EXIT_Y)));

	// �� ���� (����: ���� �ϴ� / ���߿�: �ⱸ���� �����Ÿ� �̻��� �������� ���� �� ����)
	Enemy* enemy = new Enemy(Vector2(1, MAZE_HEIGHT - 2));
	float enemySpeed = (stageLevel - 1) * ENEMY_SPEED_INCREMENT;
	enemy->SetMoveSpeed(enemySpeed);

	AddActor(enemy);

	// �÷��̾� ���� (����: ���� ��� / ���߿�: �ⱸ���� �����Ÿ� �̻��� �������� ���� �� ����)
	AddActor(new Player(Vector2(1, 1)));
}

// �ּ� 2���� �̻��� ��� ���� �Լ� (��, ��)
void MazeLevel::EnsureUpDownPaths()
{
	// �ⱸ���� ���� 2���� ��� Ȯ��
	bool hasLeftPath = HasValidPath(EXIT_X, EXIT_Y, EXIT_X, EXIT_Y - 2);
	bool hasRightPath = HasValidPath(EXIT_X, EXIT_Y, EXIT_X, EXIT_Y + 2);

	// ��� ��ΰ� ������ ����
	if (!hasLeftPath)
	{
		mazeData[EXIT_Y - 1][EXIT_X] = MazeCellType::Ground;
		mazeData[EXIT_Y - 2][EXIT_X] = MazeCellType::Ground;
	}

	// �ϴ� ��ΰ� ������ ����
	if (!hasRightPath)
	{
		mazeData[EXIT_Y + 1][EXIT_X] = MazeCellType::Ground;
		mazeData[EXIT_Y + 2][EXIT_X] = MazeCellType::Ground;
	}
}

// ��� ���� Ȯ�� �Լ�
bool MazeLevel::HasValidPath(int startX, int startY, int targetX, int targetY)
{
	// �߰� ������ ������ Ȯ��
	if (targetX == startX - 2 && targetY == startY)			// ����
	{
		return mazeData[startY][startX - 1] == MazeCellType::Ground &&
			mazeData[startY][startX - 2] == MazeCellType::Ground;
	}
	else if (targetX == startX + 2 && targetY == startY)	// ����
	{
		return mazeData[startY][startX + 1] == MazeCellType::Ground && 
			   mazeData[startY][startX + 2] == MazeCellType::Ground;
	}
	else if (targetX == startX && targetY == startY + 1)	// �ϴ�
	{
		return mazeData[startY + 1][startX] == MazeCellType::Ground &&
			mazeData[startY + 2][startX] == MazeCellType::Ground;
	}
	else if (targetX == startX && targetY == startY - 1)	// ���
	{
		return mazeData[startY - 1][startX] == MazeCellType::Ground &&
			mazeData[startY - 2][startX] == MazeCellType::Ground;
	}

	return false;
}

// �̷� ����� �Լ�
void MazeLevel::RegenerateMaze()
{
	// ���� Actor�� ��� ����
	for (Actor* actor : actors)
	{
		SafeDelete(actor);
	}
	actors.clear();

	isStageClear = false;
	isShowingPath = false;

	// �̷� ����
	GenerateMaze();

	// �ʱ� ������ (��, ��, �ⱸ)
	InitialRender();
}

void MazeLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// Todo: (���߿� ����) RŰ�� ������ �̷� �����
	if (Input::Get().GetKeyDown('R'))
	{
		RegenerateMaze();
	}

	CheckStageClear();
	UpdatePathVisualization(deltaTime);
}

// �ʱ� ������ (��, ��, �ⱸ): �ʱ⿡ ������ �� ��ȭ�� ���� ���� (������ ���ϸ� ���� ��ȭ�� �ִµ� ��ĭ�̶� ���������)
void MazeLevel::InitialRender()
{
	for (Actor* const actor : actors)
	{
		if (actor->As<Wall>() || actor->As<Ground>() || actor->As<Target>())
		{
			actor->Render();
		}
	}
}

void MazeLevel::Render()
{
	// �޴����� ���ƿ� ��� (��, ��, �ⱸ) �緻����
	if (needsStaticRerender)
	{
		InitialRender();
		needsStaticRerender = false;
	}

	if (isStageClear)
	{
		Utils::SetConsolePosition({ MAZE_WIDTH + 10, MAZE_HEIGHT / 2});
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::Green));
		std::cout << "  Stage Clear!!!  \n";

		Sleep(1000);
		system("cls");

		stageLevel++;
		if (stageLevel >= 6)
		{
			// Todo: ���� ���ᰡ �ƴ� ���� �޴��� ���� �߰� ����
			Engine::Get().Quit();
		}

		RegenerateMaze();
	}
	else
	{
		// �÷��̾�, ��, Path ã�Ƽ� ������
		for (Actor* const actor : actors)
		{
			if (actor->As<Player>() || actor->As<Enemy>() || actor->As<Path>())
			{
				actor->Render();
			}
		}
	}
}

// �ⱸ�� �÷��̾ ���� ���� �ߴ��� Ȯ��
void MazeLevel::CheckStageClear()
{
	Target* target = nullptr;
	Enemy* enemy = nullptr;
	Player* player = nullptr;

	for (Actor* const actor : actors)
	{
		if (actor->As<Target>())
		{
			target = actor->As<Target>();
		}
		else if (actor->As<Enemy>())
		{
			enemy = actor->As<Enemy>();
		}
		else if (actor->As<Player>())
		{
			player = actor->As<Player>();
		}
	}

	// Player�� Target�� �����ߴٸ�
	if (player && target && player->Position() == target->Position() && !isShowingPath)
	{
		if (enemy)
		{
			enemy->StopMovement();
		}

		StartPathVisualization();
	}
	// Enemy�� Target�� �����ߴٸ�
	else if (enemy && target && enemy->Position() == target->Position())
	{
		Utils::SetConsolePosition({ MAZE_WIDTH + 10, MAZE_HEIGHT / 2 });
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::Red));
		std::cout << "  Game Over!!!  \n";;

		Sleep(1000);
		system("cls");

		// Todo: ���� ���ᰡ �ƴ� ���� �޴��� ���� �߰� ����
		Engine::Get().Quit();
	}
}

// Stage Clear�� Enemy�� Target������ ���� ��� Path Actor�� ǥ���ϴ� �Լ�
void MazeLevel::StartPathVisualization()
{
	isShowingPath = true;

	// Enemy���� Path Actor�� ���� ��û
	for (Actor* const actor : actors)
	{
		if (Enemy* enemy = actor->As<Enemy>())
		{
			enemy->CreatePathActors();
			break;
		}
	}
}

// Path Animation ���� �Լ�
void MazeLevel::UpdatePathVisualization(float deltaTime)
{
	if (!isShowingPath) 
	{
		return;
	}

	pathShowTimer += deltaTime;

	if (pathShowTimer >= PATH_SHOW_INTERVAL)
	{
		pathShowTimer = 0.0f;

		// Enemy���� ���� Path ���� ��û
		for (Actor* const actor : actors)
		{
			if (Enemy* enemy = actor->As<Enemy>())
			{
				bool hasMorePaths = enemy->RemoveNextPath();
				if (!hasMorePaths)
				{
					// ��� Path�� ���ŵǸ� Stage Clear
					isShowingPath = false;
					isStageClear = true;
				}
				break;
			}
		}
	}
}

// �÷��̾� ������ ���� ���� Ȯ��
bool MazeLevel::CanPlayerMove(const Vector2& playerPosition, const Vector2& targetPosition)
{
	if (isShowingPath || isStageClear)
	{
		return false;
	}

	// �̵��Ϸ��� ��ġ�� Wall�� �ִ��� Ȯ��
	for (Actor* const actor : actors)
	{
		if (actor->Position() == targetPosition)
		{
			// Wall�̸� �̵� �Ұ�
			if (actor->As<Wall>())
			{
				return false;
			}

			// Ground or Target or Enemy
			return true;
		}
	}

	return false;
}
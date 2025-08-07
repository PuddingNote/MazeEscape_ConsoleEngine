#include "MazeLevel.h"
#include "Math/Vector2.h"
#include "Utils/Utils.h"
#include "Engine.h"

#include "Game/Game.h"
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
#include <fstream>
#include <string>

MazeLevel::MazeLevel(int width, int height) : MAZE_WIDTH(width), MAZE_HEIGHT(height)
{
	// �ⱸ(EXIT) ��ġ ���
	EXIT_X = MAZE_WIDTH - 2;
	EXIT_Y = MAZE_HEIGHT / 2;

	// ���� �迭 �Ҵ�
	mazeData = new MazeCellType * [MAZE_HEIGHT];
	for (int i = 0; i < MAZE_HEIGHT; ++i)
	{
		mazeData[i] = new MazeCellType[MAZE_WIDTH];
	}

	// MainLevel���� �� ��� �緻���� �ʿ�
	needsRerender = true;

	InitializeMaze();
}

MazeLevel::~MazeLevel()
{
	for (int i = 0; i < MAZE_HEIGHT; ++i)
	{
		SafeDeleteArray(mazeData[i]);
	}

	SafeDeleteArray(mazeData);
}



// �̷� ���� ���� ���� �Լ�
void MazeLevel::InitializeMaze()
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

	// Actor ���� (�ⱸ, ��, �÷��̾�)
	InitializeActors();

	// �ʱ� ������ (��, ��, �ⱸ)
	InitializeRender();
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



// �ʱ� ���� ���� �Լ� (�ⱸ, ��, �÷��̾�)
void MazeLevel::InitializeActors()
{
	// �ⱸ ���� (����: ���� �ߴ� / ���߿�: �߾� ��� or ���� �״��)
	AddActor(new Target(Vector2(EXIT_X, EXIT_Y)));

	// �� ���� (����: ���� �ϴ� / ���߿�: �ⱸ���� �����Ÿ� �̻��� �������� ���� �� ����)
	Enemy* enemy = new Enemy(Vector2(1, MAZE_HEIGHT - 2));
	enemy->SetMazeSize(MAZE_WIDTH, MAZE_HEIGHT);
	float enemySpeed = stageLevel * ENEMY_SPEED_INCREMENT;
	enemy->SetMoveSpeed(enemySpeed);

	AddActor(enemy);

	// �÷��̾� ���� (����: ���� ��� / ���߿�: �ⱸ���� �����Ÿ� �̻��� �������� ���� �� ����)
	AddActor(new Player(Vector2(1, 1)));
}

// �ʱ� ������ (��, ��, �ⱸ): �ʱ⿡ ������ �� ��ȭ�� ���� ����
void MazeLevel::InitializeRender()
{
	for (Actor* const actor : actors)
	{
		if (actor->As<Wall>() || actor->As<Ground>() || actor->As<Target>())
		{
			actor->Render();
		}
	}
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
	currentScore = 0;

	// �̷� ����
	InitializeMaze();

	// �ʱ� ������ (��, ��, �ⱸ)
	InitializeRender();
}



// ��ȿ�� ������ Ȯ���ϴ� �Լ� (�̷� ������ �ľ��� ����: �̷� ���� ���� �ְ�, �����ڸ��� �ƴ� ���)
bool MazeLevel::IsValidCell(int x, int y)
{
	return x > 0 && x < MAZE_WIDTH - 1 && y > 0 && y < MAZE_HEIGHT - 1;
}

// �ּ� 2���� �̻��� ��� ���� �Լ� (��, ��)
void MazeLevel::EnsureUpDownPaths()
{
	// �ⱸ���� ���� 2���� ��� Ȯ��
	bool hasUpPath = HasValidPath(EXIT_X, EXIT_Y, EXIT_X, EXIT_Y - 2);
	bool hasDownPath = HasValidPath(EXIT_X, EXIT_Y, EXIT_X, EXIT_Y + 2);

	// ��� ��ΰ� ������ ����
	if (!hasUpPath)
	{
		mazeData[EXIT_Y - 1][EXIT_X] = MazeCellType::Ground;
		mazeData[EXIT_Y - 2][EXIT_X] = MazeCellType::Ground;
	}

	// �ϴ� ��ΰ� ������ ����
	if (!hasDownPath)
	{
		mazeData[EXIT_Y + 1][EXIT_X] = MazeCellType::Ground;
		mazeData[EXIT_Y + 2][EXIT_X] = MazeCellType::Ground;
	}
}

// ��� ���� Ȯ�� �Լ�
bool MazeLevel::HasValidPath(int startX, int startY, int targetX, int targetY)
{
	// �߰� ������ ������ Ȯ��
	if (targetX == startX && targetY == startY + 1)			// �ϴ�
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



void MazeLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// Todo: RŰ�� ������ �̷� ����� (�׽�Ʈ ���Ǽ��� ���� �����Ŷ� �˾Ƹ� �ΰ� ���� ������ ����)
	if (Input::Get().GetKeyDown('R'))
	{
		RegenerateMaze();
	}

	CheckStageClear();
	UpdatePathVisualization(deltaTime);
}

void MazeLevel::Render()
{
	// �޴����� �� ��� (��, ��, �ⱸ) �緻����
	if (needsRerender)
	{
		InitializeRender();
		needsRerender = false;
	}

	Utils::SetConsolePosition({ static_cast<short>(MAZE_WIDTH + 9), 1 });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::SkyBlue));
	std::cout << "Stage: " << stageLevel + 1;

	ShowBestScore(static_cast<short>(MAZE_WIDTH + 6), 3);

	Utils::SetConsolePosition({ static_cast<short>(MAZE_WIDTH + 6), 4 });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::Yellow));
	std::cout << "Total Score: " << totalScore;

	Utils::SetConsolePosition({ static_cast<short>(MAZE_WIDTH + 6), 5 });
	std::cout << "Stage Score: " << currentScore;

	// Todo: BFS�� A* �� �ʿ�� Ȱ��ȭ
	// === Enemy ã�Ƽ� Count Result ǥ�� ===
	/*Enemy* enemy = nullptr;
	for (Actor* const actor : actors)
	{
		if (actor->As<Enemy>())
		{
			enemy = actor->As<Enemy>();
			break;
		}
	}
	if (enemy)
	{
		Utils::SetConsolePosition({ static_cast<short>(MAZE_WIDTH + 28), 1 });
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::SkyBlue));
		std::cout << "[Enemy Path Count Result]";

		Utils::SetConsolePosition({ static_cast<short>(MAZE_WIDTH + 34), 3 });
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::Yellow));
		std::cout << "BFS Count: " << enemy->GetBFSCount();

		Utils::SetConsolePosition({ static_cast<short>(MAZE_WIDTH + 35), 4 });
		std::cout << "A* Count: " << enemy->GetAStarCount();
	}*/

	if (isStageClear)
	{
		Utils::SetConsolePosition({ static_cast<short>(MAZE_WIDTH + 23), 4 });
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::Purple));
		std::cout << "+" << currentScore;

		Utils::SetConsolePosition({ static_cast<short>(MAZE_WIDTH + 7), 7 });
		std::cout << "Stage Clear!!!";

		totalScore += currentScore;

		Sleep(2000);
		system("cls");

		stageLevel++;
		if (stageLevel >= MAX_STAGE)
		{
			Utils::SetConsolePosition({ 5, 3 });
			Utils::SetConsoleTextColor(static_cast<WORD>(Color::Yellow));
			std::cout << "*** ALL STAGES COMPLETED! ***";

			Utils::SetConsolePosition({ 7, 4 });
			std::cout << "Returning to Main Menu...";

			// �ְ����� ���� �� ǥ��
			SaveBestScore();
			ShowFinalScore();

			Sleep(3000);
			system("cls");

			Game::Get().GoMainMenuLevel();
		}
		else
		{
			currentScore = 0;
			RegenerateMaze();
		}
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
				AddCurrentScore();
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



// �÷��̾� ������ ���� ���� Ȯ�� �Լ�
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

// �ⱸ�� �÷��̾ ���� ���� �ߴ��� Ȯ���ϴ� �Լ�
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
		Utils::SetConsolePosition({ static_cast<short>(MAZE_WIDTH + 7), 7 });
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::Purple));
		std::cout << "Game Over!!!";

		Sleep(2000);
		system("cls");

		Utils::SetConsolePosition({ 10, 3 });
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::Yellow));
		std::cout << "*** GAME OVER! ***";;

		Utils::SetConsolePosition({ 7, 4 });
		std::cout << "Returning to Main Menu...";

		// �ְ����� ���� �� ǥ��
		SaveBestScore();
		ShowFinalScore();

		Sleep(3000);
		system("cls");

		Game::Get().GoMainMenuLevel();
	}
}

// Total ���� �߰� �Լ�
void MazeLevel::AddTotalScore()
{
	totalScore += currentScore;
}

// Stage ���� �߰� �Լ�
void MazeLevel::AddCurrentScore()
{
	currentScore++;
}



// ��忡 ���� ��ȣ ��ȯ �Լ�
int MazeLevel::GetModeLineNumber() const
{
	if (MAZE_WIDTH == 21 && MAZE_HEIGHT == 11)
	{
		return 0;
	}
	else if (MAZE_WIDTH == 33 && MAZE_HEIGHT == 15)
	{
		return 1;
	}
	else
	{
		return 2;
	}
}

// �ְ����� ���� �Լ�
void MazeLevel::SaveBestScore()
{
	std::string fileName = "../Settings/BestScore.txt";
	int scores[3] = { 0, 0, 0 };

	// ���� ������ �б�
	std::ifstream file(fileName);
	if (file.is_open())
	{
		for (int i = 0; i < 3; i++)
		{
			file >> scores[i];
		}
		file.close();
	}

	// ���� ����� ���� ������Ʈ (�� ���� ������ ����)
	int modeIndex = GetModeLineNumber();
	if (totalScore > scores[modeIndex])
	{
		scores[modeIndex] = totalScore;

		// ���Ͽ� ��� ���� ����
		std::ofstream file(fileName);
		if (file.is_open())
		{
			for (int i = 0; i < 3; i++)
			{
				file << scores[i] << std::endl;
			}
			file.close();
		}

		Utils::SetConsolePosition({ 30, 6 });
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::Red));
		std::cout << "New Record!!";
	}
}

// �ְ����� �ε� �Լ�
int MazeLevel::LoadBestScore() const
{
	std::string fileName = "../Settings/BestScore.txt";
	int scores[3] = { 0, 0, 0 };

	std::ifstream file(fileName);
	if (file.is_open())
	{
		for (int i = 0; i < 3; i++)
		{
			file >> scores[i];
		}
		file.close();
	}

	int modeIndex = GetModeLineNumber();
	return scores[modeIndex];
}

// Best Score ǥ�� �Լ�
void MazeLevel::ShowBestScore(short x, short y)
{
	Utils::SetConsolePosition({ x, y });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::SkyBlue));
	std::cout << "Best  Score: " << LoadBestScore();
}

// ���� ������ Best Score ǥ�� �Լ�
void MazeLevel::ShowFinalScore()
{
	ShowBestScore(11, 6);

	Utils::SetConsolePosition({ 11, 7 });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::Yellow));
	std::cout << "Total Score: " << totalScore;
}
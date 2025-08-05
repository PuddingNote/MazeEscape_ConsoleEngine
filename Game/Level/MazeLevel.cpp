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

// 미로 생성 관련 모음 함수
void MazeLevel::GenerateMaze()
{
	// 랜덤 시드 초기화
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// 미로 데이터 초기화 (모두 벽으로 초기화)
	for (int y = 0; y < MAZE_HEIGHT; ++y)
	{
		for (int x = 0; x < MAZE_WIDTH; ++x)
		{
			mazeData[y][x] = MazeCellType::Wall;
		}
	}

	// DFS로 미로 생성 (출구에서 시작하여 미로 생성)
	GenerateMazeDFS(EXIT_X, EXIT_Y);

	// 최소 2갈래 이상 경로 보장 (상, 하)
	EnsureUpDownPaths();

	// Actor 생성 (길, 벽)
	for (int y = 0; y < MAZE_HEIGHT; ++y)
	{
		for (int x = 0; x < MAZE_WIDTH; ++x)
		{
			if (mazeData[y][x] == MazeCellType::Wall)
			{
				AddActor(new Wall(Vector2(x, y)));		// 벽
			}
			else
			{
				AddActor(new Ground(Vector2(x, y)));	// 길
			}
		}
	}

	// Actor 생성 (적, 플레이어)
	InitializeActors();

	// 초기 렌더링 (벽, 길, 출구)
	InitialRender();
}

// DFS를 사용한 랜덤 미로 생성 함수
void MazeLevel::GenerateMazeDFS(int x, int y)
{
	// 현재 위치를 길로 설정 (방문한 셀을 길로 만들어 표시: 이 위치는 이미 처리되었음을 표시)
	mazeData[y][x] = MazeCellType::Ground;

	// 4방향 (상, 하, 좌, 우) 랜덤 순서로 탐색 (2칸씩 이동하는 이유: 벽과 길이 번갈아 나타나기 때문)
	int directions[4][2] = { {0, -2}, {0, 2}, {-2, 0}, {2, 0} };

	// 방향을 랜덤하게 섞기 (Fisher-Yates 셔플: 매번 다른 순서로 방향을 탐색해서 미로의 다양성 보장)
	int indices[4] = { 0, 1, 2, 3 };
	for (int i = 3; i > 0; --i)
	{
		int j = std::rand() % (i + 1);
		std::swap(indices[i], indices[j]);
	}

	// 4가지 방향을 무작위 순서로 하나씩 시도
	for (int i = 0; i < 4; ++i)
	{
		int dx = directions[indices[i]][0];
		int dy = directions[indices[i]][1];
		int newX = x + dx;
		int newY = y + dy;

		// 유효한 셀이고 아직 방문하지 않은 경우
		if (IsValidCell(newX, newY) && mazeData[newY][newX] == MazeCellType::Wall)
		{
			// 현재 위치와 새로운 위치의 가운데 벽을 길로 바꿔 길을 뚫음
			mazeData[y + dy / 2][x + dx / 2] = MazeCellType::Ground;

			// 새로운 위치에서 재귀적으로 길 뚫기 진행
			GenerateMazeDFS(newX, newY);
		}
	}
}

// 유효한 셀인지 확인하는 함수 (미로 생성시 파악을 위해: 미로 범위 내에 있고, 가장자리가 아닌 경우)
bool MazeLevel::IsValidCell(int x, int y)
{
	return x > 0 && x < MAZE_WIDTH - 1 && y > 0 && y < MAZE_HEIGHT - 1;
}

// 초기 액터 생성 함수 (출구, 적, 플레이어)
void MazeLevel::InitializeActors()
{
	// 출구 생성 (현재: 우측 중단 / 나중에: 중앙 상단 or 기존 그대로)
	AddActor(new Target(Vector2(EXIT_X, EXIT_Y)));

	// 적 생성 (현재: 좌측 하단 / 나중에: 출구에서 일정거리 이상의 랜덤으로 설정 할 예정)
	Enemy* enemy = new Enemy(Vector2(1, MAZE_HEIGHT - 2));
	float enemySpeed = (stageLevel - 1) * ENEMY_SPEED_INCREMENT;
	enemy->SetMoveSpeed(enemySpeed);

	AddActor(enemy);

	// 플레이어 생성 (현재: 좌측 상단 / 나중에: 출구에서 일정거리 이상의 랜덤으로 설정 할 예정)
	AddActor(new Player(Vector2(1, 1)));
}

// 최소 2갈래 이상의 경로 보장 함수 (상, 하)
void MazeLevel::EnsureUpDownPaths()
{
	// 출구에서 상하 2방향 경로 확인
	bool hasLeftPath = HasValidPath(EXIT_X, EXIT_Y, EXIT_X, EXIT_Y - 2);
	bool hasRightPath = HasValidPath(EXIT_X, EXIT_Y, EXIT_X, EXIT_Y + 2);

	// 상단 경로가 없으면 생성
	if (!hasLeftPath)
	{
		mazeData[EXIT_Y - 1][EXIT_X] = MazeCellType::Ground;
		mazeData[EXIT_Y - 2][EXIT_X] = MazeCellType::Ground;
	}

	// 하단 경로가 없으면 생성
	if (!hasRightPath)
	{
		mazeData[EXIT_Y + 1][EXIT_X] = MazeCellType::Ground;
		mazeData[EXIT_Y + 2][EXIT_X] = MazeCellType::Ground;
	}
}

// 경로 존재 확인 함수
bool MazeLevel::HasValidPath(int startX, int startY, int targetX, int targetY)
{
	// 중간 지점이 길인지 확인
	if (targetX == startX - 2 && targetY == startY)			// 좌측
	{
		return mazeData[startY][startX - 1] == MazeCellType::Ground &&
			mazeData[startY][startX - 2] == MazeCellType::Ground;
	}
	else if (targetX == startX + 2 && targetY == startY)	// 우측
	{
		return mazeData[startY][startX + 1] == MazeCellType::Ground && 
			   mazeData[startY][startX + 2] == MazeCellType::Ground;
	}
	else if (targetX == startX && targetY == startY + 1)	// 하단
	{
		return mazeData[startY + 1][startX] == MazeCellType::Ground &&
			mazeData[startY + 2][startX] == MazeCellType::Ground;
	}
	else if (targetX == startX && targetY == startY - 1)	// 상단
	{
		return mazeData[startY - 1][startX] == MazeCellType::Ground &&
			mazeData[startY - 2][startX] == MazeCellType::Ground;
	}

	return false;
}

// 미로 재생성 함수
void MazeLevel::RegenerateMaze()
{
	// 기존 Actor들 모두 삭제
	for (Actor* actor : actors)
	{
		SafeDelete(actor);
	}
	actors.clear();

	isStageClear = false;
	isShowingPath = false;

	// 미로 생성
	GenerateMaze();

	// 초기 렌더링 (벽, 길, 출구)
	InitialRender();
}

void MazeLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// Todo: (나중에 삭제) R키를 누르면 미로 재생성
	if (Input::Get().GetKeyDown('R'))
	{
		RegenerateMaze();
	}

	CheckStageClear();
	UpdatePathVisualization(deltaTime);
}

// 초기 렌더링 (벽, 길, 출구): 초기에 생성된 후 변화가 없기 때문 (엄밀히 말하면 길은 변화가 있는데 빈칸이라 상관없을듯)
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
	// 메뉴에서 돌아온 경우 (벽, 길, 출구) 재렌더링
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
			// Todo: 게임 종료가 아닌 메인 메뉴로 가기 추가 예정
			Engine::Get().Quit();
		}

		RegenerateMaze();
	}
	else
	{
		// 플레이어, 적, Path 찾아서 렌더링
		for (Actor* const actor : actors)
		{
			if (actor->As<Player>() || actor->As<Enemy>() || actor->As<Path>())
			{
				actor->Render();
			}
		}
	}
}

// 출구에 플레이어나 적이 도달 했는지 확인
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

	// Player가 Target에 도달했다면
	if (player && target && player->Position() == target->Position() && !isShowingPath)
	{
		if (enemy)
		{
			enemy->StopMovement();
		}

		StartPathVisualization();
	}
	// Enemy가 Target에 도달했다면
	else if (enemy && target && enemy->Position() == target->Position())
	{
		Utils::SetConsolePosition({ MAZE_WIDTH + 10, MAZE_HEIGHT / 2 });
		Utils::SetConsoleTextColor(static_cast<WORD>(Color::Red));
		std::cout << "  Game Over!!!  \n";;

		Sleep(1000);
		system("cls");

		// Todo: 게임 종료가 아닌 메인 메뉴로 가기 추가 예정
		Engine::Get().Quit();
	}
}

// Stage Clear시 Enemy의 Target까지의 남은 경로 Path Actor로 표현하는 함수
void MazeLevel::StartPathVisualization()
{
	isShowingPath = true;

	// Enemy에게 Path Actor들 생성 요청
	for (Actor* const actor : actors)
	{
		if (Enemy* enemy = actor->As<Enemy>())
		{
			enemy->CreatePathActors();
			break;
		}
	}
}

// Path Animation 진행 함수
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

		// Enemy에게 다음 Path 제거 요청
		for (Actor* const actor : actors)
		{
			if (Enemy* enemy = actor->As<Enemy>())
			{
				bool hasMorePaths = enemy->RemoveNextPath();
				if (!hasMorePaths)
				{
					// 모든 Path가 제거되면 Stage Clear
					isShowingPath = false;
					isStageClear = true;
				}
				break;
			}
		}
	}
}

// 플레이어 움직임 가능 여부 확인
bool MazeLevel::CanPlayerMove(const Vector2& playerPosition, const Vector2& targetPosition)
{
	if (isShowingPath || isStageClear)
	{
		return false;
	}

	// 이동하려는 위치에 Wall이 있는지 확인
	for (Actor* const actor : actors)
	{
		if (actor->Position() == targetPosition)
		{
			// Wall이면 이동 불가
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
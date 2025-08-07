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
	// 출구(EXIT) 위치 계산
	EXIT_X = MAZE_WIDTH - 2;
	EXIT_Y = MAZE_HEIGHT / 2;

	// 동적 배열 할당
	mazeData = new MazeCellType * [MAZE_HEIGHT];
	for (int i = 0; i < MAZE_HEIGHT; ++i)
	{
		mazeData[i] = new MazeCellType[MAZE_WIDTH];
	}

	// MainLevel에서 온 경우 재렌더링 필요
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



// 미로 생성 관련 모음 함수
void MazeLevel::InitializeMaze()
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

	// Actor 생성 (벽, 길)
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

	// Actor 생성 (출구, 적, 플레이어)
	InitializeActors();

	// 초기 렌더링 (벽, 길, 출구)
	InitializeRender();
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



// 초기 액터 생성 함수 (출구, 적, 플레이어)
void MazeLevel::InitializeActors()
{
	// 출구 생성 (현재: 우측 중단 / 나중에: 중앙 상단 or 기존 그대로)
	AddActor(new Target(Vector2(EXIT_X, EXIT_Y)));

	// 적 생성 (현재: 좌측 하단 / 나중에: 출구에서 일정거리 이상의 랜덤으로 설정 할 예정)
	Enemy* enemy = new Enemy(Vector2(1, MAZE_HEIGHT - 2));
	enemy->SetMazeSize(MAZE_WIDTH, MAZE_HEIGHT);
	float enemySpeed = stageLevel * ENEMY_SPEED_INCREMENT;
	enemy->SetMoveSpeed(enemySpeed);

	AddActor(enemy);

	// 플레이어 생성 (현재: 좌측 상단 / 나중에: 출구에서 일정거리 이상의 랜덤으로 설정 할 예정)
	AddActor(new Player(Vector2(1, 1)));
}

// 초기 렌더링 (벽, 길, 출구): 초기에 생성된 후 변화가 없기 때문
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
	currentScore = 0;

	// 미로 생성
	InitializeMaze();

	// 초기 렌더링 (벽, 길, 출구)
	InitializeRender();
}



// 유효한 셀인지 확인하는 함수 (미로 생성시 파악을 위해: 미로 범위 내에 있고, 가장자리가 아닌 경우)
bool MazeLevel::IsValidCell(int x, int y)
{
	return x > 0 && x < MAZE_WIDTH - 1 && y > 0 && y < MAZE_HEIGHT - 1;
}

// 최소 2갈래 이상의 경로 보장 함수 (상, 하)
void MazeLevel::EnsureUpDownPaths()
{
	// 출구에서 상하 2방향 경로 확인
	bool hasUpPath = HasValidPath(EXIT_X, EXIT_Y, EXIT_X, EXIT_Y - 2);
	bool hasDownPath = HasValidPath(EXIT_X, EXIT_Y, EXIT_X, EXIT_Y + 2);

	// 상단 경로가 없으면 생성
	if (!hasUpPath)
	{
		mazeData[EXIT_Y - 1][EXIT_X] = MazeCellType::Ground;
		mazeData[EXIT_Y - 2][EXIT_X] = MazeCellType::Ground;
	}

	// 하단 경로가 없으면 생성
	if (!hasDownPath)
	{
		mazeData[EXIT_Y + 1][EXIT_X] = MazeCellType::Ground;
		mazeData[EXIT_Y + 2][EXIT_X] = MazeCellType::Ground;
	}
}

// 경로 존재 확인 함수
bool MazeLevel::HasValidPath(int startX, int startY, int targetX, int targetY)
{
	// 중간 지점이 길인지 확인
	if (targetX == startX && targetY == startY + 1)			// 하단
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



void MazeLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// Todo: R키를 누르면 미로 재생성 (테스트 편의성을 위해 넣은거라 알아만 두고 따로 없애진 않음)
	if (Input::Get().GetKeyDown('R'))
	{
		RegenerateMaze();
	}

	CheckStageClear();
	UpdatePathVisualization(deltaTime);
}

void MazeLevel::Render()
{
	// 메뉴에서 온 경우 (벽, 길, 출구) 재렌더링
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

	// Todo: BFS와 A* 비교 필요시 활성화
	// === Enemy 찾아서 Count Result 표시 ===
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

			// 최고점수 저장 및 표시
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
				AddCurrentScore();
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



// 플레이어 움직임 가능 여부 확인 함수
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

// 출구에 플레이어나 적이 도달 했는지 확인하는 함수
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

		// 최고점수 저장 및 표시
		SaveBestScore();
		ShowFinalScore();

		Sleep(3000);
		system("cls");

		Game::Get().GoMainMenuLevel();
	}
}

// Total 점수 추가 함수
void MazeLevel::AddTotalScore()
{
	totalScore += currentScore;
}

// Stage 점수 추가 함수
void MazeLevel::AddCurrentScore()
{
	currentScore++;
}



// 모드에 따른 번호 반환 함수
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

// 최고점수 저장 함수
void MazeLevel::SaveBestScore()
{
	std::string fileName = "../Settings/BestScore.txt";
	int scores[3] = { 0, 0, 0 };

	// 기존 점수들 읽기
	std::ifstream file(fileName);
	if (file.is_open())
	{
		for (int i = 0; i < 3; i++)
		{
			file >> scores[i];
		}
		file.close();
	}

	// 현재 모드의 점수 업데이트 (더 높은 점수일 때만)
	int modeIndex = GetModeLineNumber();
	if (totalScore > scores[modeIndex])
	{
		scores[modeIndex] = totalScore;

		// 파일에 모든 점수 저장
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

// 최고점수 로드 함수
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

// Best Score 표시 함수
void MazeLevel::ShowBestScore(short x, short y)
{
	Utils::SetConsolePosition({ x, y });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::SkyBlue));
	std::cout << "Best  Score: " << LoadBestScore();
}

// 최종 점수와 Best Score 표시 함수
void MazeLevel::ShowFinalScore()
{
	ShowBestScore(11, 6);

	Utils::SetConsolePosition({ 11, 7 });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::Yellow));
	std::cout << "Total Score: " << totalScore;
}
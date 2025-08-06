#include "Game.h"

#include "Level/MainMenuLevel.h"
#include "Level/MazeLevel.h"
#include "Level/MenuLevel.h"

Game* Game::instance = nullptr;

Game::Game()
{
	instance = this;

	// 메인 레벨 추가
	AddLevel(new MainMenuLevel());

	// 메뉴 레벨 생성
	menuLevel = new MenuLevel();
}

Game::~Game()
{
	CleanUp();
}

void Game::ToggleMenu()
{
	// 화면 정리
	system("cls");

	// 토글 처리
	showMenu = !showMenu;

	if (showMenu)
	{
		// 현재 레벨을 백업하고 메뉴로 전환
		backLevel = mainLevel;	// 게임 레벨을 뒤로 밀기
		mainLevel = menuLevel;	// 메뉴 레벨을 메인 레벨로 설정
	}
	else
	{
		// 백업된 레벨로 복원
		mainLevel = backLevel;
		backLevel = nullptr;
	}
}

void Game::GoMainMenuLevel()
{
	system("cls");
	SafeSwitchLevel(new MainMenuLevel());
}

void Game::StartGame(int width, int height)
{
	system("cls");
	SafeSwitchLevel(new MazeLevel(width, height));
}

void Game::SafeSwitchLevel(Level* newLevel)
{
	// 현재 메인 레벨이 메뉴가 아닌 경우에만 삭제
	if (mainLevel && mainLevel != menuLevel)
	{
		SafeDelete(mainLevel);
	}

	// 백업 레벨도 메뉴가 아닌 경우에만 삭제
	if (backLevel && backLevel != menuLevel)
	{
		SafeDelete(backLevel);
	}

	mainLevel = newLevel;
	backLevel = nullptr;
	showMenu = false;
}

void Game::CleanUp()
{
	// 메뉴가 활성화 됐는지 여부에 따라 정리 작업 처리
	if (showMenu)
	{
		// 게임 레벨 제거
		SafeDelete(backLevel);
		mainLevel = nullptr;
	}

	SafeDelete(menuLevel);
	Engine::CleanUp();
}

Game& Game::Get()
{
	return *instance;
}
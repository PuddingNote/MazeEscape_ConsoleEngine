#include "Game.h"

#include "Level/MazeLevel.h"
#include "Level/MenuLevel.h"

Game* Game::instance = nullptr;

Game::Game()
{
	instance = this;

	// 메인 레벨 추가
	AddLevel(new MazeLevel());

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
		// 게임 레벨을 뒤로 밀기
		backLevel = mainLevel;

		// 메뉴 레벨을 메인 레벨로 설정
		mainLevel = menuLevel;
	}
	else
	{
		// 게임 레벨을 메인 레벨로 설정
		mainLevel = backLevel;
	}
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
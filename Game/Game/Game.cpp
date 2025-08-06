#include "Game.h"

#include "Level/MainMenuLevel.h"
#include "Level/MazeLevel.h"
#include "Level/MenuLevel.h"

Game* Game::instance = nullptr;

Game::Game()
{
	instance = this;

	// ���� ���� �߰�
	AddLevel(new MainMenuLevel());

	// �޴� ���� ����
	menuLevel = new MenuLevel();
}

Game::~Game()
{
	CleanUp();
}

void Game::ToggleMenu()
{
	// ȭ�� ����
	system("cls");

	// ��� ó��
	showMenu = !showMenu;

	if (showMenu)
	{
		// ���� ������ ����ϰ� �޴��� ��ȯ
		backLevel = mainLevel;	// ���� ������ �ڷ� �б�
		mainLevel = menuLevel;	// �޴� ������ ���� ������ ����
	}
	else
	{
		// ����� ������ ����
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
	// ���� ���� ������ �޴��� �ƴ� ��쿡�� ����
	if (mainLevel && mainLevel != menuLevel)
	{
		SafeDelete(mainLevel);
	}

	// ��� ������ �޴��� �ƴ� ��쿡�� ����
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
	// �޴��� Ȱ��ȭ �ƴ��� ���ο� ���� ���� �۾� ó��
	if (showMenu)
	{
		// ���� ���� ����
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
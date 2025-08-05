#include "Game.h"

#include "Level/MazeLevel.h"
#include "Level/MenuLevel.h"

Game* Game::instance = nullptr;

Game::Game()
{
	instance = this;

	// ���� ���� �߰�
	AddLevel(new MazeLevel());

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
		// ���� ������ �ڷ� �б�
		backLevel = mainLevel;

		// �޴� ������ ���� ������ ����
		mainLevel = menuLevel;
	}
	else
	{
		// ���� ������ ���� ������ ����
		mainLevel = backLevel;
	}
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
#include "MainMenuLevel.h"
#include "../Game//Game.h"
#include "Utils/Utils.h"
#include "Input.h"

#include <iostream>

MainMenuLevel::MainMenuLevel()
{
	// �޴� ������ �߰�
	items.emplace_back(new MainMenuItem("Easy Mode", []()
		{
			Game::Get().StartGame(21, 11);
		}
	));

	items.emplace_back(new MainMenuItem("Normal Mode", []()
		{
			Game::Get().StartGame(33, 15);
		}
	));

	items.emplace_back(new MainMenuItem("Hard Mode", []()
		{
			Game::Get().StartGame(51, 19);
		}
	));

	items.emplace_back(new MainMenuItem("Quit Game", []()
		{
			Game::Get().Quit();
		}
	));

	// ������ �� �̸� ����
	length = static_cast<int>(items.size());
}

MainMenuLevel::~MainMenuLevel()
{
	for (MainMenuItem* item : items)
	{
		SafeDelete(item);
	}

	items.clear();
}

void MainMenuLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	// �Է� ó��
	if (Input::Get().GetKeyDown(VK_UP))
	{
		currentIndex = (currentIndex - 1 + length) % length;
	}

	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		currentIndex = (currentIndex + 1) % length;
	}

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		// �޴� �������� �����ϰ� �ִ� �Լ� �ݹ�
		items[currentIndex]->onSelected();
	}

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		// Game�� ��� �޴� ��� �߰� �� ȣ��
		Game::Get().ToggleMenu();

		// �޴� �ε��� �ʱ�ȭ
		currentIndex = 0;
	}
}

void MainMenuLevel::Render()
{
	super::Render();

	// ���� & ��ǥ ����
	Utils::SetConsolePosition({ 0, 0 });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::SkyBlue));
	std::cout << "Maze Escape Game\n\n\n";

	Utils::SetConsoleTextColor(static_cast<WORD>(unselectedColor));
	// �޴� ������ ������
	for (int i = 0; i < length; ++i)
	{
		// ������ ���� Ȯ��
		Color textColor = (i == currentIndex) ? selectedColor : unselectedColor;

		// ���� ����
		Utils::SetConsoleTextColor(static_cast<WORD>(textColor));

		// �޴� �ؽ�Ʈ ���
		std::cout << items[i]->menuText << "\n";
	}
}
#include "MenuLevel.h"
#include "../Game//Game.h"
#include "Utils/Utils.h"
#include "Input.h"

#include <iostream>

MenuLevel::MenuLevel()
{
	// �޴� ������ �߰�
	items.emplace_back(new MenuItem("Resume Game", []()
		{
			Game::Get().ToggleMenu();
		}
	));

	items.emplace_back(new MenuItem("Go Main Menu", []()
		{
			Game::Get().GoMainMenuLevel();
		}
	));

	length = static_cast<int>(items.size());	// ������ �� �̸� ����
}

MenuLevel::~MenuLevel()
{
	for (MenuItem* item : items)
	{
		SafeDelete(item);
	}

	items.clear();
}

void MenuLevel::Tick(float deltaTime)
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

void MenuLevel::Render()
{
	super::Render();

	// ���� & ��ǥ ����
	Utils::SetConsolePosition({ 0, 0 });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::SkyBlue));
	std::cout << "Game Paused\n\n\n";

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
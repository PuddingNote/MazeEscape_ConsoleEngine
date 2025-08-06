#include "MainMenuLevel.h"
#include "../Game//Game.h"
#include "Utils/Utils.h"
#include "Input.h"

#include <iostream>

MainMenuLevel::MainMenuLevel()
{
	// 메뉴 아이템 추가
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

	// 아이템 수 미리 저장
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

	// 입력 처리
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
		// 메뉴 아이템이 저장하고 있는 함수 콜백
		items[currentIndex]->onSelected();
	}

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		// Game에 토글 메뉴 기능 추가 후 호출
		Game::Get().ToggleMenu();

		// 메뉴 인덱스 초기화
		currentIndex = 0;
	}
}

void MainMenuLevel::Render()
{
	super::Render();

	// 색상 & 좌표 정리
	Utils::SetConsolePosition({ 0, 0 });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::SkyBlue));
	std::cout << "Maze Escape Game\n\n\n";

	Utils::SetConsoleTextColor(static_cast<WORD>(unselectedColor));
	// 메뉴 아이템 렌더링
	for (int i = 0; i < length; ++i)
	{
		// 아이템 색상 확인
		Color textColor = (i == currentIndex) ? selectedColor : unselectedColor;

		// 색상 설정
		Utils::SetConsoleTextColor(static_cast<WORD>(textColor));

		// 메뉴 텍스트 출력
		std::cout << items[i]->menuText << "\n";
	}
}
#include "MenuLevel.h"
#include "../Game//Game.h"
#include "Utils/Utils.h"
#include "Input.h"

#include <iostream>

MenuLevel::MenuLevel()
{
	// 메뉴 아이템 추가
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

	length = static_cast<int>(items.size());	// 아이템 수 미리 저장
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

void MenuLevel::Render()
{
	super::Render();

	// 색상 & 좌표 정리
	Utils::SetConsolePosition({ 0, 0 });
	Utils::SetConsoleTextColor(static_cast<WORD>(Color::SkyBlue));
	std::cout << "Game Paused\n\n\n";

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
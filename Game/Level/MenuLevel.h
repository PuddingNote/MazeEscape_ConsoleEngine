#pragma once

#include "Level/Level.h"
#include "Actor/Actor.h"

#include <vector>

struct MenuItem
{
	// 함수 포인터 선언
	typedef void (*OnSelected)();

	MenuItem(const char* text, OnSelected onSelected) : onSelected(onSelected)
	{
		size_t length = strlen(text) + 1;
		menuText = new char[length];
		strcpy_s(menuText, length, text);
	}
	~MenuItem()
	{
		SafeDeleteArray(menuText);
	}

	// 메뉴 텍스트
	char* menuText = nullptr;

	// 메뉴 선택 시 실행할 동작
	OnSelected onSelected = nullptr;
};

class MenuLevel : public Level
{
	RTTI_DECLARATIONS(MenuLevel, Level)

public:
	MenuLevel();
	~MenuLevel();

	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

private:
	// 현재 선택된 아이템의 인덱스
	int currentIndex = 0;
	
	Color selectedColor = Color::Green;		// 아이템 선택O 색상
	Color unselectedColor = Color::White;	// 아이템 선택X 색상

	// 아이템 배열
	std::vector<MenuItem*> items;

	// 메뉴 아이템 수
	int length = 0;
};
#pragma once

#include "Engine.h"

class Level;
class Game : public Engine
{
public:
	Game();
	~Game();

	// 메뉴 전환 함수
	void ToggleMenu();

	virtual void CleanUp() override;

	static Game& Get();

private:
	Level* menuLevel = nullptr;
	Level* backLevel = nullptr;

	// 현재 메뉴 레벨을 보여주고 있는지를 나타냄
	bool showMenu = false;

	static Game* instance;
};
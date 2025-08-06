#pragma once

#include "Engine.h"

class Level;
class Game : public Engine
{
public:
	Game();
	~Game();

	void ToggleMenu();						// 메뉴 전환 함수
	void GoMainMenuLevel();					// 메인화면으로 돌아가는 함수
	void StartGame(int width, int height);	// 모드별 게임 시작 함수

	virtual void CleanUp() override;
	static Game& Get();

private:
	void SafeSwitchLevel(Level* newLevel);	// 안전한 레벨 전환을 위한 함수 추가

private:
	Level* menuLevel = nullptr;
	Level* backLevel = nullptr;
	bool showMenu = false;		// 현재 메뉴 레벨을 보여주고 있는지를 나타냄

	static Game* instance;
};
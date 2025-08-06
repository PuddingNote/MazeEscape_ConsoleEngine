#pragma once

#include "Engine.h"

class Level;
class Game : public Engine
{
public:
	Game();
	~Game();

	void ToggleMenu();						// �޴� ��ȯ �Լ�
	void GoMainMenuLevel();					// ����ȭ������ ���ư��� �Լ�
	void StartGame(int width, int height);	// ��庰 ���� ���� �Լ�

	virtual void CleanUp() override;
	static Game& Get();

private:
	void SafeSwitchLevel(Level* newLevel);	// ������ ���� ��ȯ�� ���� �Լ� �߰�

private:
	Level* menuLevel = nullptr;
	Level* backLevel = nullptr;
	bool showMenu = false;		// ���� �޴� ������ �����ְ� �ִ����� ��Ÿ��

	static Game* instance;
};
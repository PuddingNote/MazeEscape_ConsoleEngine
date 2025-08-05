#pragma once

#include "Engine.h"

class Level;
class Game : public Engine
{
public:
	Game();
	~Game();

	// �޴� ��ȯ �Լ�
	void ToggleMenu();

	virtual void CleanUp() override;

	static Game& Get();

private:
	Level* menuLevel = nullptr;
	Level* backLevel = nullptr;

	// ���� �޴� ������ �����ְ� �ִ����� ��Ÿ��
	bool showMenu = false;

	static Game* instance;
};
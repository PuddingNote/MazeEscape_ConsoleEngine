#pragma once

#include "Core.h"

class Engine_API Input
{
	friend class Engine;

	struct KeyState
	{
		bool isKeyDown = false;
		bool previousKeyDown = false;
	};

public:
	Input();

	// 키 확인 함수
	bool GetKey(int keyCode);
	bool GetKeyDown(int keyCode);
	bool GetKeyUp(int keyCode);

	static Input& Get();

private:
	void ProcessInput();
	void SavePreviousKeyStates();

private:
	// 키 입력 정보 관리 변수
	KeyState keyStates[256];// = { };

	static Input* instance;
};
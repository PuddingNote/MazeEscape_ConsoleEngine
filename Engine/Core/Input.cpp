#include "Input.h"

#include <Windows.h>

// static ���� ����
Input* Input::instance = nullptr;

Input::Input()
{
	instance = this;
}

void Input::ProcessInput()
{
	// Ű �Է� Ȯ��
	for (int i = 0; i < 255; ++i)
	{
		keyStates[i].isKeyDown = GetAsyncKeyState(i) & 0x8000;
	}
}

void Input::SavePreviousKeyStates()
{
	for (int i = 0; i < 255; ++i)
	{
		keyStates[i].previousKeyDown = keyStates[i].isKeyDown;
	}
}

bool Input::GetKey(int keyCode)
{
	return keyStates[keyCode].isKeyDown;
}

bool Input::GetKeyDown(int keyCode)
{
	return !keyStates[keyCode].previousKeyDown
		&& keyStates[keyCode].isKeyDown;
}

bool Input::GetKeyUp(int keyCode)
{
	return keyStates[keyCode].previousKeyDown
		&& !keyStates[keyCode].isKeyDown;
}

Input& Input::Get()
{
	return *instance;
}
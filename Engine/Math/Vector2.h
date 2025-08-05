#pragma once

#include "Core.h"

class Engine_API Vector2
{
public:
	Vector2(int x = 0, int y = 0);
	~Vector2();

	Vector2 operator+(const Vector2& other) const;
	Vector2 operator-(const Vector2& other) const;
	bool operator==(const Vector2& other) const;

	// 문자열로 출력
	const char* ToString();

	static Vector2 Zero;
	static Vector2 One;
	static Vector2 Up;
	static Vector2 Right;

public:
	int x = 0;
	int y = 0;

private:
	char* buffer = nullptr;

};
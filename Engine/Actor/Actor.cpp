#include "Actor.h"
#include "Utils/Utils.h"
#include "Engine.h"

#include <Windows.h>
#include <iostream>

Actor::Actor(const char image, Color color, const Vector2& position) : image(image), color(color), position(position)
{

}

Actor::~Actor()
{

}

void Actor::BeginPlay()
{
	hasBeganPlay = true;
}

void Actor::Tick(float deltaTime)
{

}

// 그리기 함수
void Actor::Render()
{
	// 커서 위치 값 생성
	COORD coord;
	coord.X = (short)position.x;
	coord.Y = (short)position.y;

	// 커서 이동
	Utils::SetConsolePosition(coord);

	// 색상 설정
	Utils::SetConsoleTextColor(static_cast<WORD>(color));

	// 그리기
	std::cout << image;
}

void Actor::SetPosition(const Vector2& newPosition)
{
	// 커서 위치 값 생성
	COORD coord;
	coord.X = (short)position.x;
	coord.Y = (short)position.y;

	// 커서 이동
	Utils::SetConsolePosition(coord);

	// 공백으로 지우기
	std::cout << ' ';

	// 새 위치로 설정
	position = newPosition;
}

Vector2 Actor::Position() const
{
	return position;
}

void Actor::SetSortingOrder(unsigned int sortingOrder)
{
	this->sortingOrder = sortingOrder;
}

void Actor::SetOwner(Level* newOwner)
{
	owner = newOwner;
}

Level* Actor::GetOwner()
{
	return owner;
}

void Actor::QuitGame()
{
	Engine::Get().Quit();
}
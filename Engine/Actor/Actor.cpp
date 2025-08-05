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

// �׸��� �Լ�
void Actor::Render()
{
	// Ŀ�� ��ġ �� ����
	COORD coord;
	coord.X = (short)position.x;
	coord.Y = (short)position.y;

	// Ŀ�� �̵�
	Utils::SetConsolePosition(coord);

	// ���� ����
	Utils::SetConsoleTextColor(static_cast<WORD>(color));

	// �׸���
	std::cout << image;
}

void Actor::SetPosition(const Vector2& newPosition)
{
	// Ŀ�� ��ġ �� ����
	COORD coord;
	coord.X = (short)position.x;
	coord.Y = (short)position.y;

	// Ŀ�� �̵�
	Utils::SetConsolePosition(coord);

	// �������� �����
	std::cout << ' ';

	// �� ��ġ�� ����
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
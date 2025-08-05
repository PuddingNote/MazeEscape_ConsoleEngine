#pragma once

#include "Actor/Actor.h"

class Player : public Actor
{
	RTTI_DECLARATIONS(Player, Actor)

public:
	Player(const Vector2& position);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;

private:
	class ICanPlayerMove* canPlayerMoveInterface = nullptr;		// 인터페이스 참조 변수

};
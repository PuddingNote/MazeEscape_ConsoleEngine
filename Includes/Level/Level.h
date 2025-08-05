#pragma once

#include "Core.h"
#include "RTTI.h"

#include <vector>

class Actor;
class Engine_API Level : public RTTI
{
	RTTI_DECLARATIONS(Level, RTTI)

public:
	Level();
	virtual ~Level();
	
	// Level�� Actor�� �߰��� �� ���
	void AddActor(Actor* newActor);

	// Level�� Actor�� ������ �� ���
	void RemoveActor(Actor* actor);

	// ���� �̺�Ʈ �Լ�
	virtual void BeginPlay();
	virtual void Tick(float deltaTime);
	virtual void Render();

	const std::vector<Actor*>& GetActors() const { return actors; }

private:
	void SortActorsBySortingOrder();

protected:
	// ������ ��ġ�� ��� Actor�� �����ϴ� �迭
	std::vector<Actor*> actors;
};
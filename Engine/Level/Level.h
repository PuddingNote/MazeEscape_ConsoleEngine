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
	
	// Level에 Actor를 추가할 때 사용
	void AddActor(Actor* newActor);

	// Level에 Actor를 삭제할 때 사용
	void RemoveActor(Actor* actor);

	// 엔진 이벤트 함수
	virtual void BeginPlay();
	virtual void Tick(float deltaTime);
	virtual void Render();

	const std::vector<Actor*>& GetActors() const { return actors; }

private:
	void SortActorsBySortingOrder();

protected:
	// 레벨에 배치된 모든 Actor를 관리하는 배열
	std::vector<Actor*> actors;
};
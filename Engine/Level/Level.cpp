#include "Level.h"
#include "Actor/Actor.h"

Level::Level()
{

}

Level::~Level()
{
	// 메모리 해제
	for (Actor* actor : actors)
	{
		SafeDelete(actor);
	}

	// std::vector 정리
	actors.clear();
}



void Level::AddActor(Actor* newActor)
{
	actors.emplace_back(newActor);

	// Ownership 설정
	newActor->SetOwner(this);
}

void Level::RemoveActor(Actor* actor)
{
	auto it = std::find(actors.begin(), actors.end(), actor);
    if (it != actors.end())
    {
        SafeDelete(*it);
        actors.erase(it);
    }
}



void Level::BeginPlay()
{
	for (Actor* const actor : actors)
	{
		if (actor->HasBeganPlay())
		{
			continue;
		}

		actor->BeginPlay();
	}
}

void Level::Tick(float deltaTime)
{
	for (Actor* const actor : actors)
	{
		actor->Tick(deltaTime);
	}
}

void Level::Render()
{
	// 그리기 전에 정렬 순서 기준으로 정렬
	SortActorsBySortingOrder();

	// Render Pass
	for (Actor* const actor : actors)
	{
		// 검사 (같은 위치에 정렬 순서 높은 액터가 있는지 확인)
		Actor* searchedActor = nullptr;
		for (Actor* const otherActor : actors)
		{
			if (actor == otherActor)
			{
				continue;
			}

			// 위치가 같은 액터 확인
			if (actor->Position() == otherActor->Position())
			{
				// 정렬 순서 비교 후 액터 저장
				if (actor->sortingOrder < otherActor->sortingOrder)
				{
					searchedActor = otherActor;
					break;
				}
			}
		}

		// 어떤 액터와 같은 위치에 정렬 순서가 더 높은 액터가 있으면 그리지 않고 건너뛰기
		if (searchedActor)
		{
			continue;
		}

		// 드로우 콜
		actor->Render();
	}
}



void Level::SortActorsBySortingOrder()
{
	for (int i = 0; i < (int)actors.size(); ++i)
	{
		for (int j = 0; j < (int)actors.size() - 1; ++j)
		{
			//if (actors[j]->sortingOrder < actors[j + 1]->sortingOreder)
			if (actors[j]->sortingOrder > actors[j + 1]->sortingOrder)
			{
				std::swap(actors[j], actors[j + 1]);
			}
		}
	}
}
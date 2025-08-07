#include "Level.h"
#include "Actor/Actor.h"

Level::Level()
{

}

Level::~Level()
{
	// �޸� ����
	for (Actor* actor : actors)
	{
		SafeDelete(actor);
	}

	// std::vector ����
	actors.clear();
}



void Level::AddActor(Actor* newActor)
{
	actors.emplace_back(newActor);

	// Ownership ����
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
	// �׸��� ���� ���� ���� �������� ����
	SortActorsBySortingOrder();

	// Render Pass
	for (Actor* const actor : actors)
	{
		// �˻� (���� ��ġ�� ���� ���� ���� ���Ͱ� �ִ��� Ȯ��)
		Actor* searchedActor = nullptr;
		for (Actor* const otherActor : actors)
		{
			if (actor == otherActor)
			{
				continue;
			}

			// ��ġ�� ���� ���� Ȯ��
			if (actor->Position() == otherActor->Position())
			{
				// ���� ���� �� �� ���� ����
				if (actor->sortingOrder < otherActor->sortingOrder)
				{
					searchedActor = otherActor;
					break;
				}
			}
		}

		// � ���Ϳ� ���� ��ġ�� ���� ������ �� ���� ���Ͱ� ������ �׸��� �ʰ� �ǳʶٱ�
		if (searchedActor)
		{
			continue;
		}

		// ��ο� ��
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
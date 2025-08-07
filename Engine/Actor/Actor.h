#pragma once

#include "Core.h"
#include "Math/Vector2.h"
#include "RTTI.h"

enum class Color : int
{
	Blue = 1,
	Green = 2,
	Red = 4,
	White = Red | Green | Blue,
	Yellow = Red | Green,
	Purple = Red | Blue,
	SkyBlue = Green | Blue,
	Intensity = 8
};

class Level;
class Engine_API Actor : public RTTI
{
	friend class Level;
	RTTI_DECLARATIONS(Actor, RTTI)

public:
	Actor(
		const char image = ' ',
		Color color = Color::White,
		const Vector2& position = Vector2::Zero
	);
	virtual ~Actor();

	// 이벤트 함수
	virtual void BeginPlay();			// 초기화 목적으로 처음에 1번만 호출
	virtual void Tick(float deltaTime);	// 프레임 마다 호출
	virtual void Render();				// 그리기 함수

	// BeginPlay 호출 여부 확인
	inline bool HasBeganPlay() const { return hasBeganPlay; }

	// 위치 설정/값 읽는 함수
	void SetPosition(const Vector2& newPosition);
	Vector2 Position() const;

	// Sorting Order 설정
	void SetSortingOrder(unsigned int sortingOrder);

	// Ownership 설정
	void SetOwner(Level* newOwner);
	Level* GetOwner();

	// 게임 종료 요청 함수
	void QuitGame();

private:
	Vector2 position;				// 개체의 위치

	char image = ' ';				// 그릴 값
	Color color;					// 텍스트 색상 값

	bool hasBeganPlay = false;		// BeginPlay 호출 확인

	unsigned int sortingOrder = 0;	// 정렬 순서

	Level* owner = nullptr;			// 소유 레벨
};
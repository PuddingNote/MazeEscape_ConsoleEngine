#pragma once

#include "Math/Vector2.h"

// 플레이어 이동 가능 여부 판별 인터페이스
class ICanPlayerMove
{
public:
	virtual bool CanPlayerMove(const Vector2& playerPosition, const Vector2& targetPosition) = 0;
};
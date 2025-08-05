#pragma once

#include "Math/Vector2.h"

// �÷��̾� �̵� ���� ���� �Ǻ� �������̽�
class ICanPlayerMove
{
public:
	virtual bool CanPlayerMove(const Vector2& playerPosition, const Vector2& targetPosition) = 0;
};
#pragma once

#include "Actor/Actor.h"

class Path : public Actor
{
	RTTI_DECLARATIONS(Path, Actor)

public:
	Path(const Vector2& position);

};
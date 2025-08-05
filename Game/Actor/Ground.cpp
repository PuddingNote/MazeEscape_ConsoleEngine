#include "Ground.h"

Ground::Ground(const Vector2& position) : Actor(' ', Color::White, position)
{
	SetSortingOrder(0);
}
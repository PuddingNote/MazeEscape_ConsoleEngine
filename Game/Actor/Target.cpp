#include "Target.h"

Target::Target(const Vector2& position) : Actor('T', Color::Red, position)
{
	SetSortingOrder(1);
}
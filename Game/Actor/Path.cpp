#include "Path.h"

Path::Path(const Vector2& position) : Actor('.', Color::Red, position)
{
	SetSortingOrder(1);
}
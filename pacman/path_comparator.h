#pragma once
#include "path.h"

class PathComparator
{
public:
	bool operator() (const Path& p1, const Path& p2) const
	{
		return p1.length <= p2.length;
	}
};
#pragma once
#include "edge.h"
#include <list>
using namespace std;
struct Path
{
	list<Edge> path;
	int length;

	Path() {
		length = 0;
	}

	Path(const Path& p)
	{
		length = p.length;
		
		for (auto edge = p.path.begin(); edge != p.path.end(); edge++)
		{
			path.push_back(*edge);
		}
	}

	friend bool operator<(Path a, Path b)
	{
		return a.length > b.length;
	}

};
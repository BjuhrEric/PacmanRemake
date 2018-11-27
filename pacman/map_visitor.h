#pragma once
#include "visitable_map.h"

class MapVisitor {

public:
	virtual void setVisitableMap(VisitableMap * map) = 0;

};
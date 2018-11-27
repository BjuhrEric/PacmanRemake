#pragma once
class VisitableScoreKeeper {
public:
	virtual void addScore(int amount) = 0;
	virtual void addMaxScore(int amount) = 0;
};
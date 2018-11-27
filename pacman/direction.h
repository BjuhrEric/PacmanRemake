#pragma once
enum Direction {
	left = 0, right = 1, up = 2, down = 3, none = 4
};

struct Translation {
	const int x, y;
	static const Translation * const t_left;
	static const Translation * const t_right;
	static const Translation * const t_up;
	static const Translation * const t_down;
	static const Translation * const t_none;
private:
	Translation(const int x, const int y);
};

namespace dir {
	static const Translation * getTranslation(Direction dir)
	{
		switch (dir)
		{
		case left: return Translation::t_left;
		case right: return Translation::t_right;
		case up: return Translation::t_up;
		case down: return Translation::t_down;
		case none:
		default: break;
		}
		return Translation::t_none;
	}

	static const Direction getOpposite(const Direction dir) {
		switch (dir) {
		case up: return down;
		case down: return up;
		case left: return right;
		case right: return left;
		}
		return none;
	}
}
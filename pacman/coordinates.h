#pragma once
struct Coordinates
{
	int x, y;

	int getX() {
		return x;
	}
	int getY() {
		return y;
	}

	Coordinates() {}
	Coordinates(int x, int y) : x(x), y(y) {}
	Coordinates(const Coordinates &copy) : Coordinates(copy.x, copy.y) {}
	
	Coordinates& operator-=(const Coordinates& c) {
		x -= c.x;
		y -= c.y;
		return *this;
	}

	Coordinates& operator+=(const Coordinates& c) {
		x += c.x;
		y += c.y;
		return *this;
	}

	Coordinates& operator*=(int n) {
		x *= n;
		y *= n;
		return *this;
	}

	bool operator!=(Coordinates& c) {
		return x != c.x || y != c.y;
	}

	bool operator==(Coordinates& c) {
		return x == c.x && y == c.y;
	}

};
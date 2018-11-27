#include "direction.h"

Translation::Translation(int x, int y) : x(x), y(y) {}

const Translation * const Translation::t_left = new Translation(-1, 0);
const Translation * const Translation::t_right = new Translation(1, 0);
const Translation * const Translation::t_up = new Translation(0, -1);
const Translation * const Translation::t_down = new Translation(0, 1);
const Translation * const Translation::t_none = new Translation(0, 0);
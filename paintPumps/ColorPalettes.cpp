#include "ColorPattern.cpp"
#include "Colors.h"
#include <vector>

ColorPattern RAINBOW_CYCLE = ColorPattern(
  std::vector<Color>({RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE}), 
  std::vector<long>({2000, 3000, 2000, 3000, 2000, 3000}));
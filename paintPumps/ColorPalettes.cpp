#include "ColorPattern.cpp"
#include "Colors.h"
#include <vector>

ColorPattern RAINBOW_CYCLE = ColorPattern(
  std::vector<Color>({RED, ORANGE, YELLOW, GREEN, BLUE, PURPLE}), 
  std::vector<long>({2000, 3000, 2000, 3000, 2000, 3000}));

ColorPattern TEST_4_COLOR = ColorPattern(
  std::vector<Color>({RED, ORANGE, YELLOW, GREEN, YELLOW}), 
  std::vector<long>({6000, 4000, 6000, 4000, 6000}));

ColorPattern LIGHT_COLOR = ColorPattern(
  std::vector<Color>({RED, ORANGE, YELLOW, ORANGE}), 
  std::vector<long>({3000, 6000, 8000, 6000}));

ColorPattern WARM_COLOR = ColorPattern(
  std::vector<Color>({RED, PURPLE, RED, ORANGE}), 
  std::vector<long>({12000, 8000, 12000, 8000}));

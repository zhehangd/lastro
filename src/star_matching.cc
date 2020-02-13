#include "star_matching.h"

#include <algorithm>
#include <cmath>

namespace lastro {

void DrawStarPattern(cv::Mat &canvas, int x, int y, const StarList &stars,
                     cv::Scalar color) {
  for (const auto &star : stars) {
    cv::line(canvas, {x, y}, {star.x, star.y}, color);
  }
}

}

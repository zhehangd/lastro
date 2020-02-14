#include "star_matching.h"

#include <algorithm>
#include <cmath>

namespace lastro {

Feature GenerateFeature(int x, int y, const StarList &star_list,
                        double max_radius) {
  auto w = RES_ANGLE;
  auto h = RES_LENGTH;
  float angle_delta = 2 * CV_PI / w;
  float dist_delta = (double)max_radius / h;
  
  //cv::Mat spectrum(dsize, CV_64F, cv::Scalar(0));
  //std::string name = fmt::format("spectrum_{:04}_{:04}", (int)src.x, (int)src.y);
  
  // Find the brightest star as the angle reference
  std::size_t max_val_idx = 0;
  double max_val = 0;
  
  for (std::size_t i = 0; i < star_list.size(); ++i) {
    const auto &star = star_list[i];
    if (star.value > max_val) {
      max_val_idx = i;
      max_val = star.value;
    }
  }

  auto A0_x = star_list[max_val_idx].x;
  auto A0_y = star_list[max_val_idx].y;
  double A0 = std::atan2(A0_y - y, A0_x - x);
  
  Feature feat = {};
  for (const auto &star : star_list) {
    double dx = star.x - x;
    double dy = star.y - y;

    // Angle
    double angle = std::atan2(dy, dx) - A0;
    angle = std::fmod(angle, 2 * CV_PI);
    if (angle < 0) {angle += 2 * CV_PI;}

    double angle_f, angle_i;
    angle_f = std::modf(angle / angle_delta, &angle_i);

    double dist = std::sqrt(dx * dx + dy * dy);
    if (dist > max_radius) {continue;}

    double dist_f, dist_i;
    dist_f = std::modf(dist / dist_delta, &dist_i);

    int r0 = (int)dist_i;
    int c0 = (int)angle_i;
    int r1 = (r0 + 1) % h;
    int c1 = (c0 + 1) % w;
    
    double val = 255.0;
    double val00 = val * (1 - angle_f) * (1 - dist_f);
    double val01 = val * angle_f * (1 - dist_f);
    double val10 = val * (1 - angle_f) * dist_f;
    double val11 = val * angle_f * dist_f;
    
    feat[r0 * w + c0] += val00;
    feat[r0 * w + c1] += val01;
    feat[r1 * w + c0] += val10;
    feat[r1 * w + c1] += val11;
  }
  return feat;
}

void DrawStarPattern(cv::Mat &canvas, int x, int y, const StarList &stars,
                     cv::Scalar color) {
  for (const auto &star : stars) {
    cv::line(canvas, {x, y}, {star.x, star.y}, color);
  }
}

}

#include "star_detection_and_matching.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>

#include <glog/logging.h>
#include <fmt/format.h>

#include "dwt2.h"

namespace {

cv::Rect ClipRect(cv::Rect rect, cv::Size size) {
  int sx = std::max(rect.x, 0);
  int sy = std::max(rect.y, 0);
  int ex = std::min(rect.x + rect.width,  size.width);
  int ey = std::min(rect.y + rect.height, size.height);
  return cv::Rect(sx, sy, ex - sx, ey - sy);
}

Feature GenerateFeature(const BasicStar &ref_star,
                        const StarIndex &pattern_stars, double max_radius) {
  auto w = RES_ANGLE;
  auto h = RES_LENGTH;
  float angle_delta = 2 * CV_PI / w;
  float dist_delta = (double)max_radius / h;
  
  //cv::Mat spectrum(dsize, CV_64F, cv::Scalar(0));
  //std::string name = fmt::format("spectrum_{:04}_{:04}", (int)src.x, (int)src.y);
  
  // Find the brightest star as the angle reference
  std::size_t max_val_idx = 0;
  double max_val = 0;
  
  for (std::size_t i = 0; i < pattern_stars.size(); ++i) {
    const auto &star = pattern_stars[i];
    if (ref_star.x == star.x && ref_star.y == star.y) {continue;}
    if (star.value > max_val) {
      max_val_idx = i;
      max_val = star.value;
    }
  }
  
  double A0 = std::atan2(pattern_stars[max_val_idx].y - ref_star.y,
                         pattern_stars[max_val_idx].x - ref_star.x);
  Feature feat = {};
  for (const auto &star : pattern_stars) {
    double dx = star.x - ref_star.x;
    double dy = star.y - ref_star.y;

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

}

cv::Mat CreateStarMask(cv::Mat image, double thres) {
  CHECK_EQ(image.channels(), 1);
  CHECK_EQ(image.dims, 2);
  
  cv::Mat hp_image;
  DWT2HighPass(image, hp_image);
  if (thres <= 0) {return hp_image;}
  
  double max_val;
  cv::minMaxLoc(hp_image, nullptr, &max_val, nullptr, nullptr);
  cv::threshold(hp_image, hp_image, thres * max_val, 255, cv::THRESH_BINARY);
  
  cv::Mat mask;
  hp_image.convertTo(mask, CV_8UC1);
  return mask;
}

void DetectStars(const cv::Mat &image, const cv::Mat &mask,
                 StarIndex *star_index) {
  cv::Mat labels, stats, centroids;
  int num_components = cv::connectedComponentsWithStats(
    mask, labels, stats, centroids, 8, CV_16U, cv::CCL_DEFAULT);
  int num_star_candidates = num_components - 1;
  
  star_index->resize(0);
  star_index->reserve(num_star_candidates);
  
  for (int i = 1; i <= num_star_candidates; ++i) {
    auto *centroid_row = centroids.ptr<double>(i);
    auto *stats_row = stats.ptr<std::int32_t>(i);
    
    int x = static_cast<int>(std::round(centroid_row[0]));
    int y = static_cast<int>(std::round(centroid_row[1]));
    
    // The number of pixels in the thresholded component
    std::int32_t area = stats_row[cv::CC_STAT_AREA];
    
    // Bounding box of the thresholded component
    cv::Rect bbox(stats_row[cv::CC_STAT_LEFT], stats_row[cv::CC_STAT_TOP],
                  stats_row[cv::CC_STAT_WIDTH], stats_row[cv::CC_STAT_HEIGHT]);
    auto bbox_len = (bbox.width + bbox.height) / 2;
    
    // Sum of the pixel values
    cv::Scalar color = cv::sum(image(bbox));
    double value = color[0] + color[1] + color[2];
    
    // Test the circularity:
    if (area * 2 < bbox.area()) {continue;} // component should be a circle
    
    if (bbox_len >= 4) {
      int delta = std::abs(bbox.width - bbox.height);
      if (delta > bbox_len / 4) {continue;} // bbox should be a square
    }
    
    star_index->push_back({x, y, value});
  }
  std::sort(star_index->begin(), star_index->end(),
            [](auto &a, auto &b) {return a.value > b.value;});
}

void SaveStarIndex(std::string filename, const StarIndex &star_index) {
  std::ofstream ofs(filename);
  for (const auto &star : star_index) {
    ofs << fmt::format("{:5} {:5} {}\n", star.x, star.y, star.value);
  }
  ofs.close();
}

std::vector<FeaturedStar> ExtractStarPatterns(
    const StarIndex &star_index, int max_num_ref_stars, double win_radius) {
  CHECK_GT(max_num_ref_stars, 0);
  int num_ref_stars = std::min(
    max_num_ref_stars, static_cast<int>(star_index.size()));
  std::vector<FeaturedStar> feat_stars;
  feat_stars.reserve(num_ref_stars);
  for (int k = 0; k < num_ref_stars; ++k) {
    const auto &ref_star = star_index[k]; // star_index should be sorted
    StarIndex pattern_stars;
    for (int i = 0; i < static_cast<int>(star_index.size()); ++i) {
      if (k == i) {continue;}
      const auto &star = star_index[i];
      int dx = std::abs(star.x - ref_star.x);
      int dy = std::abs(star.y - ref_star.y);
      if (dx > win_radius || dy > win_radius) {continue;}
      pattern_stars.push_back(star);
    }
    auto feat = GenerateFeature(ref_star, pattern_stars, win_radius);
    feat_stars.emplace_back(ref_star, feat);
  }
  return feat_stars;
}

#include "star_detection.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tuple>

#include <glog/logging.h>
#include <fmt/format.h>

#include "dwt2.h"

namespace lastro {

namespace {

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

void DetectStarsFromMask(const cv::Mat &image, const cv::Mat &mask,
                         StarList *star_index) {
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

void SaveStarList(std::string filename, const StarList &star_list) {
  std::ofstream ofs(filename);
  for (const auto &star : star_list) {
    ofs << fmt::format("{:5} {:5} {}\n", star.pos.x, star.pos.y, star.value);
  }
  ofs.close();
}

void LoadStarList(std::string filename, StarList &star_list) {
  star_list.clear();
  std::ifstream ifs(filename);
  std::string line;
  int idx = 0;
  while (std::getline(ifs, line)) {
    std::array<double, 3> nums;
    for (std::size_t i = 0; i < nums.size(); ++i) {
      std::size_t next_pos;
      nums[i] = std::stod(line, &next_pos);
      line = line.substr(next_pos);
    }
    int x = static_cast<int>(nums[0]);
    int y = static_cast<int>(nums[1]);
    double value = nums[2];
    ++idx;
    star_list.push_back({x, y, value});
  }
}

void FilterStarsByDistance(
    const StarList &stars_in, Coords pos, StarList &stars_out,
    double max_radius, double min_radius) {
  double dist2_min = min_radius * min_radius;
  double dist2_max = max_radius * max_radius;
  StarList selected;
  if (&stars_in != &stars_out) {selected.swap(stars_out);}
  selected.clear();
  std::copy_if(stars_in.begin(), stars_in.end(), std::back_inserter(selected),
    [pos, dist2_min, dist2_max](const auto &star) {
      auto dx = star.pos.x - pos.x;
      auto dy = star.pos.y - pos.y;
      double dist2 = static_cast<double>(dx * dx + dy * dy);
      return dist2 >= dist2_min && dist2 <= dist2_max;
  });
  selected.swap(stars_out);
}

void FilterStarsByBrightness(
    const StarList &stars_in, StarList &stars_out, int N) {
  std::size_t num = std::min(stars_in.size(), static_cast<std::size_t>(N));
  if (num == 0) {stars_out.clear(); return;}
  
  StarList selected;
  if (&stars_in != &stars_out) {selected.swap(stars_out);}
  selected.reserve(num + 1);
  selected.clear();
  selected.assign(num, {});
  for (const auto &star : stars_in) {
    if (star.value < selected.back().value) {continue;}
    auto it = std::upper_bound(
      selected.begin(), selected.end(), star.value,
      [](auto v, auto &s) {return v > s.value;});
    if (it != selected.end()) {
      selected.insert(it, star);
      selected.pop_back();
    }
  }
  selected.swap(stars_out);
}

}
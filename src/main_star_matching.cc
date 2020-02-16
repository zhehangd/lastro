#include "main_star_detection.h"

#include <memory>

#include <glog/logging.h>
#include <opencv2/opencv.hpp>

#include "utilities.h"
#include "star_detection.h"
#include "star_matching.h"

namespace lastro {
namespace {

void MakeFeatureList(const StarList &star_list, int num_ref_stars, int win_radius) {
  StarList ref_star_list;
  FilterStarsByBrightness(star_list, ref_star_list, num_ref_stars);
  for (std::size_t i = 0; i < ref_star_list.size(); ++i) {
    const auto &ref_star = ref_star_list[i];
    StarList nearby_star_list;
    FilterStarsByDistance(
      star_list, ref_star.pos, nearby_star_list, win_radius);
    auto feat = GenerateFeature(
      ref_star.pos, nearby_star_list, win_radius);
  }
}

cv::Mat EstimateAffineMatrix(cv::Mat from, cv::Mat to) {
  cv::Mat T;
  cv::solve(to, from, T, cv::DECOMP_NORMAL);
  T = T.t()(cv::Rect(0, 0, 3, 2));
  return T;
}

void StarMactchingDev(void) {
  
  cv::Mat image1 = cv::imread("data/IMG_4513_8b.tif");
  CHECK(image1.data != nullptr);
  cv::Mat image2 = cv::imread("data/IMG_4533_8b.tif");
  CHECK(image2.data != nullptr);
  
  StarList star_list1;
  LoadStarList("data/IMG_4513_starlist.txt", star_list1);
  StarList star_list2;
  LoadStarList("data/IMG_4533_starlist.txt", star_list2);
  
  std::vector<MatchPoint> mpts = MatchStar(star_list1, star_list2, 7.0);
  
  int x1 = 0, y1 = 0;
  int x2 = image1.cols, y2 = 0;
  int H = image1.rows;
  int W = image1.cols + image2.cols;
  cv::Mat canvas(H, W, CV_8UC3);
  image1.copyTo(canvas(cv::Rect(cv::Point(x1, y1), image1.size())));
  image2.copyTo(canvas(cv::Rect(cv::Point(x2, y2), image2.size())));
  
  for (auto pair : mpts) {
    cv::Point p1(x1 + pair.a.xi(), y1 + pair.a.yi());
    cv::Point p2(x2 + pair.b.xi(), y2 + pair.b.yi());
    cv::line(canvas, p1, p2, cv::Scalar(0, 255, 0));
  }
  
  cv::imwrite("canvas.jpg", canvas);
  
  cv::Mat mfrom(mpts.size(), 3, CV_64F);
  cv::Mat mto(mpts.size(), 3, CV_64F);
  for (std::size_t i = 0; i < mpts.size(); ++i) {
    mto.at<double>(i, 0) = mpts[i].a.x;
    mto.at<double>(i, 1) = mpts[i].a.y;
    mto.at<double>(i, 2) = 1.0;
    mfrom.at<double>(i, 0) = mpts[i].b.x;
    mfrom.at<double>(i, 1) = mpts[i].b.y;
    mfrom.at<double>(i, 2) = 1.0;
  }
  cv::Mat T = EstimateAffineMatrix(mfrom, mto);
  cv::Mat dst;
  cv::warpAffine(image2, dst, T, image2.size(), cv::INTER_CUBIC | cv::WARP_INVERSE_MAP);
  cv::imwrite("out.tif", dst);
}
 
void RegisterStarMactchingDev(CLI::App &main_app) {
  CLI::App &app = *main_app.add_subcommand("starmatchdev");
  
  auto callback = []() {
    StarMactchingDev();
  };
  
  app.parse_complete_callback(callback);
}

} // namespace {}

void RegisterStarMatchingSubcommands(CLI::App &main_app) {
  RegisterStarMactchingDev(main_app);
  
}

}

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

void StarMactchingDev(void) {
  
  cv::Mat image = cv::imread("data/orion.jpg");
  CHECK(image.data != nullptr);
  
  StarList star_list;
  LoadStarList("data/orion_starlist.txt", star_list);
  
  StarList ref_star_list;
  FilterStarsByBrightness(star_list, ref_star_list, 20);
  
  int h = ref_star_list.size() * (RES_LENGTH + 1);
  int w = RES_ANGLE;
  cv::Mat spectrum(h, w, CV_64F);
  
  for (std::size_t i = 0; i < ref_star_list.size(); ++i) {
    const auto &ref_star = ref_star_list[i];
    StarList nearby_star_list;
    FilterStarsByDistance(star_list, ref_star.pos, nearby_star_list, 200);
    for (const auto &star : nearby_star_list) {
      cv::line(image, ref_star.pos.cvPoint(), star.pos.cvPoint(),
               cv::Scalar(0, 255, 0));
    }
    auto feat = GenerateFeature(ref_star.pos, nearby_star_list, 200);
    for (int r = 0; r < RES_LENGTH; ++r) {
      for (int c = 0; c < RES_ANGLE; ++c) {
        int rr = i * (1 + RES_LENGTH) + r;
        int cc = c;
        spectrum.at<double>(rr, cc) = feat[r * RES_ANGLE + c];
      }
    }
    spectrum.row(i * (1 + RES_LENGTH) + RES_LENGTH).setTo({255});
  }
  cv::imwrite("out.jpg", image);
  cv::imwrite("spectrum.png", spectrum);
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

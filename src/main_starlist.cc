#include "main_starmask.h"

#include <memory>

#include <glog/logging.h>
#include <opencv2/opencv.hpp>

#include "common.h"
#include "star_detection_and_matching.h"

namespace lastro {
namespace starlist {

struct Config {
  
  // Input expsoure image
  std::string exposure_image_file;
  
  // Output mask/raw image
  std::string mask_image_file;
  
  // Output mask/raw image
  std::string star_list_file;
};

void MainStarList(const Config &cfg) {
  
  std::string image_filename = cfg.exposure_image_file;
  cv::Mat image = cv::imread(image_filename, cv::IMREAD_UNCHANGED);
  CHECK(image.data != nullptr);
  cv::Mat mask = cv::imread(cfg.mask_image_file, cv::IMREAD_GRAYSCALE);
  CHECK(mask.data != nullptr);
  
  if (image.channels() > 1) {
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
  }
  
  StarIndex star_index;
  DetectStars(image, mask, &star_index);
  
  std::string out_filename;
  if (!cfg.star_list_file.empty()) {
    out_filename = cfg.star_list_file;
  } else {
    out_filename = GenerateFilename(image_filename, ".", "_starlist.txt");
  }
  
  SaveStarIndex(out_filename, star_index);
}
  
void RegisterSubcommand(CLI::App &main_app) {
  auto cfg = std::make_shared<Config>();
  CLI::App &app = *main_app.add_subcommand("starlist");
  
  app.add_option("EXPOSURE", cfg->exposure_image_file,
    "Exposure image for star extraction")->required();
  
  app.add_option("STARMASK", cfg->mask_image_file,
    "Mask image for star extraction")->required();
    
  app.add_option("-o,--output", cfg->star_list_file,
    "Output file for the generated star list.");
  
  auto callback = [cfg]() {
    MainStarList(*cfg);
  };
  
  app.parse_complete_callback(callback);
}

}
}

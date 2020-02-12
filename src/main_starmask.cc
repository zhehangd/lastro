#include "main_starmask.h"

#include <memory>

#include <glog/logging.h>
#include <opencv2/opencv.hpp>

#include "common.h"
#include "star_detection_and_matching.h"

namespace lastro {
namespace starmask {

struct Config {
  
  // Input expsoure image
  std::string exposure_image_file;
  
  // Output mask/raw image
  std::string mask_image_file;
  
  // If set save the raw image before thresholding instead of the binary mask
  bool save_raw = false;
  
  // Threshold to generate the binary mask, in percentage of the maximum
  // of the pixel value in the exposure image.
  double threshold = 0.1;
  
  // Constant value added to all pixels.
  // It has effect only if you choose save the raw image.
  double offset = 0;
};

void MainStarMask(const Config &cfg) {
  
  std::string filename = cfg.exposure_image_file;
  LOG(INFO) << "Reading exposure image " << filename;
  cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED);
  
  if (image.channels() > 1) {
    LOG(INFO) << "The image has more than one channel, use the first one";
    cv::extractChannel(image, image, 0);
  }
  
  std::string out_filename;
  if (!cfg.mask_image_file.empty()) {
    out_filename = cfg.mask_image_file;
  } else {
    out_filename = GenerateFilename(filename, ".", "_starmask.tif");
  }
  
  if (cfg.save_raw) {
    LOG(INFO) << "Generating the image after the high-pass filter"
      "instead of the binary mask";
    image = CreateStarMask(image, -1);
    image.convertTo(image, image.depth(), 1, cfg.offset);
  } else {
    LOG(INFO) << "Generating the star mask with threshold " << cfg.threshold;
    image = CreateStarMask(image, cfg.threshold);
  }
  LOG(INFO) << "Saving image to " << out_filename;
  cv::imwrite(out_filename, image);
}
  
void RegisterSubcommand(CLI::App &main_app) {
  auto cfg = std::make_shared<Config>();
  CLI::App &app = *main_app.add_subcommand("starmask");
  
  app.add_option("EXPOSURE", cfg->exposure_image_file,
    "Exposure image for making star mask")->required();
  
  app.add_flag("-r,--raw", cfg->save_raw,
    "Save the raw image before thresholding instead of the binary mask");
  
  app.add_option("-t,--threshold", cfg->threshold,
    "Threshold to generate the binary mask, in percentage of the maximum\n"
    "of the pixel value in the exposure image")->default_val(0.1);
  
  app.add_option("-c,--offset", cfg->offset,
    "Constant value added to all pixels. \n"
    "It has effect only if you choose save the raw image.")->default_val(0.0);
    
  app.add_option("-o,--output", cfg->mask_image_file,
    "Output file for the generated mask/raw image.");
  
  auto callback = [cfg]() {
    MainStarMask(*cfg);
  };
  
  app.parse_complete_callback(callback);
}

}
}

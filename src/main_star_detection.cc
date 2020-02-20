#include "main_star_detection.h"

#include <memory>

#include <fmt/format.h>
#include <glog/logging.h>
#include <opencv2/opencv.hpp>

#include "utilities.h"
#include "star_detection.h"

namespace lastro {
namespace {

struct MakeStarMaskConfig {
  
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

void MakeStarMaskMain(const MakeStarMaskConfig &cfg) {
  
  std::string filename = cfg.exposure_image_file;
  LOG(INFO) << "Reading exposure image " << filename;
  cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED);
  auto depth = image.depth();
  
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
    image.convertTo(image, depth, 1, cfg.offset);
  } else {
    LOG(INFO) << "Generating the star mask with threshold " << cfg.threshold;
    image = CreateStarMask(image, cfg.threshold);
  }
  LOG(INFO) << "Saving image to " << out_filename;
  cv::imwrite(out_filename, image);
}


struct MakeStarListConfig {
  
  // Input expsoure image
  std::string exposure_image_file;
  
  // Output mask/raw image
  std::string mask_image_file;
  
  // Output mask/raw image
  std::string star_list_file;
};

void MakeStarListMain(const MakeStarListConfig &cfg) {
  
  std::string image_filename = cfg.exposure_image_file;
  cv::Mat image = cv::imread(image_filename, cv::IMREAD_UNCHANGED);
  CHECK(image.data != nullptr);
  cv::Mat mask = cv::imread(cfg.mask_image_file, cv::IMREAD_GRAYSCALE);
  CHECK(mask.data != nullptr);
  
  if (image.channels() > 1) {
    cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
  }
  
  StarList star_index;
  DetectStarsFromMask(image, mask, &star_index);
  
  std::string out_filename;
  if (!cfg.star_list_file.empty()) {
    out_filename = cfg.star_list_file;
  } else {
    out_filename = GenerateFilename(image_filename, ".", "_starlist.txt");
  }
  
  SaveStarList(out_filename, star_index);
}

struct DrawStarListConfig {
  
  // Input expsoure image
  std::string image_file;
  
  // Output mask/raw image
  std::string star_list_file;
  
  std::string output_image_file;
  
  // Radius of circles around stars
  double circle_radius;
  
  int num_stars;
  
  bool show_text;
};

void DrawStarListMain(const DrawStarListConfig &cfg) {
  std::string image_filename = cfg.image_file;
  cv::Mat image = cv::imread(image_filename);
  CHECK(image.data != nullptr);
  
  StarList star_list;
  LoadStarList(cfg.star_list_file, star_list);
  
  cv::Scalar color(0, 255, 0);
  for (std::size_t i = 0; i < star_list.size(); ++i) {
    const auto &star = star_list[i];
    cv::Point pt = star.pos.cvPoint();
    cv::circle(image, pt, cfg.circle_radius, color);
    if (cfg.show_text > 0) {
      cv::putText(image, fmt::format("{}", i), pt,
                  cv::FONT_HERSHEY_SIMPLEX, 1, color);
    }
    if (cfg.num_stars > 0) {
      if (static_cast<int>(i) == cfg.num_stars) {break;}
    }
  }
  
  std::string out_filename;
  if (!cfg.output_image_file.empty()) {
    out_filename = cfg.output_image_file;
  } else {
    out_filename = GenerateFilename(image_filename, ".", "_stars.jpg");
  }
  cv::imwrite(out_filename, image);
}
 
void RegisterMakeStarList(CLI::App &main_app) {
  auto cfg = std::make_shared<MakeStarListConfig>();
  CLI::App &app = *main_app.add_subcommand("starlist");
  
  app.add_option("EXPOSURE", cfg->exposure_image_file,
    "Exposure image for star extraction")->required();
  
  app.add_option("STARMASK", cfg->mask_image_file,
    "Mask image for star extraction")->required();
    
  app.add_option("-o,--output", cfg->star_list_file,
    "Output file for the generated star list.");
  
  auto callback = [cfg]() {
    MakeStarListMain(*cfg);
  };
  
  app.parse_complete_callback(callback);
}

void RegisterMakeStarMask(CLI::App &main_app) {
  auto cfg = std::make_shared<MakeStarMaskConfig>();
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
    MakeStarMaskMain(*cfg);
  };
  
  app.parse_complete_callback(callback);
}

void RegisterDrawStarList(CLI::App &main_app) {
  auto cfg = std::make_shared<DrawStarListConfig>();
  CLI::App &app = *main_app.add_subcommand("drawstarlist");
  
  app.add_option("IMAGE", cfg->image_file,
    "Image to draw")->required();
    
  app.add_option("STARLIST", cfg->star_list_file,
    "Image to draw")->required();
  
  app.add_option("-r,--circle-radius", cfg->circle_radius,
    "Radius of star circles")->default_val(5);
    
  app.add_option("-n,--num-stars", cfg->num_stars,
    "Show the brightest n stars")->default_val(0);
    
  app.add_option("-o,--output", cfg->output_image_file,
    "Output file for the generated image.");
  
  app.add_flag("-t,--show-text", cfg->show_text,
    "Draw star ids.");
  
  auto callback = [cfg]() {
    DrawStarListMain(*cfg);
  };
  
  app.parse_complete_callback(callback);
}

struct HighpassConfig {
  
  // Input image
  std::string input_image_file;
  
  // Output image
  std::string output_image_file;
  
  int level = 7;
  
  // Constant value added to all pixels.
  // It has effect only if you choose save the raw image.
  double offset = 0;
};

void HighpassMain(const HighpassConfig &cfg) {
  std::string filename = cfg.input_image_file;
  LOG(INFO) << "Reading image " << filename;
  cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED);
  LOG(INFO) << cfg.level;
  HighpassFilter(image, image, cfg.level);
  std::string out_filename = AutoFilename(
    cfg.output_image_file, filename, "_hp.tif");
  cv::imwrite(out_filename, image);
}

void RegisterHighpass(CLI::App &main_app) {
  auto cfg = std::make_shared<HighpassConfig>();
  CLI::App &app = *main_app.add_subcommand("highpass");
  
  app.add_option("IMAGE", cfg->input_image_file,
    "Input image to be processed by a high-pass filter")->required();
    
  app.add_option("-o,--output", cfg->output_image_file,
    "Output image file.");
  
  app.add_option("-n,--level", cfg->level,
    "Wavelet level");
  
  auto callback = [cfg]() {
    HighpassMain(*cfg);
  };
  
  app.parse_complete_callback(callback);
}

} // namespace {}

void RegisterStarDetectionSubcommands(CLI::App &main_app) {
  RegisterMakeStarMask(main_app);
  RegisterMakeStarList(main_app);
  RegisterDrawStarList(main_app);
  RegisterHighpass(main_app);
}

}

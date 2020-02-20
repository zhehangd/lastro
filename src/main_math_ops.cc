#include "main_math_ops.h"

#include <memory>

#include <glog/logging.h>
#include <opencv2/opencv.hpp>

#include "core.h"
#include "utilities.h"
#include "star_detection.h"
#include "star_matching.h"

namespace lastro {
namespace {

// Compute k1 * (I1 + b1) + k2 * (I2 + b2) + c

struct LinearConfig {
  
  std::string image_1_file;
  std::string image_2_file;
  std::string output_image_file;
  
  double k1 = 1.0;
  double k2 = 1.0;
  double b1 = 0.0;
  double b2 = 0.0;
  double c = 0.0;
};

void Linear(const LinearConfig &cfg) {
  cv::Mat image1 = cv::imread(cfg.image_1_file);
  CHECK(image1.data != nullptr);
  
  bool has_second_image = !cfg.image_2_file.empty();
  int depth = image1.depth();
  image1.convertTo(image1, CV_64F);
  cv::Mat dst = (image1 + cfg.b1) * cfg.k1 + cfg.c;
  
  cv::Mat image2;
  if (has_second_image) {
    image2 = cv::imread("data/IMG_4533_8b.tif");
    CHECK(image2.data != nullptr);
    image2.convertTo(image2, CV_64F);
    dst += (image2 + cfg.b2) * cfg.k2;
  }
  
  dst.convertTo(dst, depth);
  
  std::string out_filename;
  if (!cfg.output_image_file.empty()) {
    out_filename = cfg.output_image_file;
  } else {
    out_filename = GenerateFilename(cfg.image_1_file, ".", "_linear.tif");
  }
  cv::imwrite(out_filename, dst);
}
 
void RegisterLinear(CLI::App &main_app) {
  auto cfg = std::make_shared<LinearConfig>();
  CLI::App &app = *main_app.add_subcommand("linear");
  
  app.add_option("IMAGE1", cfg->image_1_file,
    "")->required();
    
  app.add_option("IMAGE2", cfg->image_2_file,
    "");
  
  app.add_option("--k1", cfg->k1,
    "")->default_val(1.0);
  
  app.add_option("--k2", cfg->k2,
    "")->default_val(1.0);
  
  app.add_option("--b1", cfg->b1,
    "")->default_val(0.0);
  
  app.add_option("--b2", cfg->b2,
    "")->default_val(0.0);
  
  app.add_option("--c", cfg->c,
    "")->default_val(0.0);
  
  app.add_option("-o,--output", cfg->output_image_file,
    "Output file for the generated image.");
  
  auto callback = [cfg]() {
    Linear(*cfg);
  };
  
  app.parse_complete_callback(callback);
}

struct LogConfig {
  std::string image_file;
  std::string output_image_file;
};

void Log(const LogConfig &cfg) {
  cv::Mat image = cv::imread(cfg.image_file);
  CHECK(image.data != nullptr);
  
  double maxval = MaxValue(image.depth());
  
  cv::Mat cache;
  image.convertTo(cache, CV_64F);
  cache += 1.0;
  cv::log(cache, cache);
  cache *= maxval / std::log(maxval);
  cache.convertTo(image, image.depth());
  std::string out_filename;
  if (!cfg.output_image_file.empty()) {
    out_filename = cfg.output_image_file;
  } else {
    out_filename = GenerateFilename(cfg.image_file, ".", "_log.tif");
  }
  cv::imwrite(out_filename, image);
}

void RegisterLog(CLI::App &main_app) {
  auto cfg = std::make_shared<LogConfig>();
  CLI::App &app = *main_app.add_subcommand("log");
  
  app.add_option("IMAGE", cfg->image_file,
    "")->required();
  
  app.add_option("-o,--output", cfg->output_image_file,
    "Output file for the generated image.");
  
  auto callback = [cfg]() {
    Log(*cfg);
  };
  
  app.parse_complete_callback(callback);
}

} // namespace {}

void RegisterMathOpsSubcommands(CLI::App &main_app) {
  RegisterLinear(main_app);
  RegisterLog(main_app);
}

}
 

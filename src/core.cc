#include "core.h"

#include <fmt/format.h>
#include <glog/logging.h>
#include <opencv2/opencv.hpp>

namespace lastro {

double MaxValue(int depth) {
  CHECK(depth == CV_8U || depth == CV_16U);
  if (depth == CV_8U) {
    return 255.0;
  } else {
    return 65535.0;
  }
}

cv::Mat ReadImage(std::string filename) {
  LOG(INFO) << "Reading image " << filename;
  cv::Mat image = cv::imread(filename, cv::IMREAD_UNCHANGED);
  CHECK(image.data != nullptr) << "Cannot open " << filename;
  return image;
}

void SaveImage(std::string filename, cv::Mat image) {
  cv::imwrite(filename, image);
}

}

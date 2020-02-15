#ifndef LANDASTRO_COMMON_H_
#define LANDASTRO_COMMON_H_

#include <string>

#include <glog/logging.h>
#include <opencv2/opencv.hpp>

namespace lastro {

std::string GenerateFilename(const std::string &path, const std::string &dir,
                             const std::string &ext);

}

#endif

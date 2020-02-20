#ifndef LANDASTRO_COMMON_H_
#define LANDASTRO_COMMON_H_

#include <string>

#include <glog/logging.h>
#include <opencv2/opencv.hpp>

namespace lastro {

std::string GenerateFilename(const std::string &path, const std::string &dir,
                             const std::string &ext);

// Helper function to make a filename.
// If src is not empty, simply returns it, otherwise the returned string
// is given by GenerateFilename(prototype, ".", suffix).
std::string AutoFilename(std::string src, std::string prototype, std::string suffix);

}

#endif

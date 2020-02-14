#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <CLI/CLI.hpp>
#include <glog/logging.h>
#include <opencv2/opencv.hpp>

#include "main_star_detection.h"
#include "main_star_matching.h"

using namespace lastro;

int main(int argc, char **argv) {
  google::SetStderrLogging(google::GLOG_INFO);
  google::InitGoogleLogging(argv[0]);
  
  CLI::App app {"Landscape astrophotography tools"};
  RegisterStarDetectionSubcommands(app);
  RegisterStarMatchingSubcommands(app);
  
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
      return app.exit(e);
  }
  
  return 0;
}

#ifndef LASTRO_CORE_H_
#define LASTRO_CORE_H_

#include <opencv2/opencv.hpp>

namespace lastro {

struct Coords {
  Coords(void) {}
  Coords(int x, int y) : x(static_cast<double>(x)), y(static_cast<double>(y)) {}
  Coords(double x, double y) : x(x), y(y) {}
  
  int xi(void) const {return static_cast<int>(std::round(x));}
  int yi(void) const {return static_cast<int>(std::round(y));}
  void xi(int x_) {x = static_cast<double>(x_);}
  void yi(int y_) {y = static_cast<double>(y_);}
  cv::Point cvPoint(void) const {return {xi(), yi()};}
  
  double x = 0;
  double y = 0;
};

}

#endif

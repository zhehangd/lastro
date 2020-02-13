#ifndef LASTRO_STAR_MATCHING_H_
#define LASTRO_STAR_MATCHING_H_

#include "star_detection.h"

namespace lastro {

void DrawStarPattern(cv::Mat &canvas, int x, int y, const StarList &stars,
                     cv::Scalar color);
//void GeneratePolarPattern

}

#endif

#ifndef LASTRO_STAR_MATCHING_H_
#define LASTRO_STAR_MATCHING_H_

#include "star_detection.h"

namespace lastro {

const int RES_ANGLE = 60;
const int RES_LENGTH = 20;

// 2D Feature representing the pattern of the stars in a local region
typedef std::array<double, RES_ANGLE * RES_LENGTH> Feature;

Feature GenerateFeature(Coords pos, const StarList &star_list,
                        double max_radius);

//void LocalStarPattern();

void DrawStarPattern(cv::Mat &canvas, int x, int y, const StarList &stars,
                     cv::Scalar color);
//void GeneratePolarPattern

}

#endif

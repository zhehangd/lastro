#ifndef LASTRO_STAR_MATCHING_H_
#define LASTRO_STAR_MATCHING_H_

#include "core.h"
#include "star_detection.h"

namespace lastro {

const int RES_ANGLE = 60;
const int RES_LENGTH = 20;
const int RES_TOTAL = RES_ANGLE * RES_LENGTH;

// 2D Feature representing the pattern of the stars in a local region
typedef std::array<double, RES_TOTAL> Feature;

double Distance(const Feature &f1, const Feature &f2);

struct MatchPoint {
  Coords a;
  Coords b;
};

struct Descriptor {
  Coords pos;
  Feature feat;
};

struct StarPatternFeature {
};

std::vector<MatchPoint> MatchStar(const StarList &ref_star_list,
                                  const StarList &tar_star_list);

std::vector<int> BruteForceMatch(const std::vector<Feature> &group1,
                                 const std::vector<Feature> &group2,
                                 double threshold = 1.0);

Feature GenerateFeature(Coords pos, const StarList &star_list,
                        double max_radius);

//void LocalStarPattern();

void DrawStarPattern(cv::Mat &canvas, int x, int y, const StarList &stars,
                     cv::Scalar color);
//void GeneratePolarPattern

}

#endif

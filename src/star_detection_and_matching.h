#ifndef ASTROTOOLS_STAR_DETECTION_AND_MATCHING_H_
#define ASTROTOOLS_STAR_DETECTION_AND_MATCHING_H_

#include <opencv2/opencv.hpp>

const int RES_ANGLE = 60;
const int RES_LENGTH = 20;

// Basic infomation of a detected star
struct BasicStar {
  int x, y; // Coordinates in the image
  double value; // Brightness of the star
};

// 2D Feature representing the pattern of the stars in a local region
typedef std::array<double, RES_ANGLE * RES_LENGTH> Feature;

//
typedef std::vector<BasicStar> StarIndex;

typedef std::vector<std::size_t> StarIDVector;

// Star feature in addition to the basic information of a star
struct FeaturedStar : public BasicStar {
  FeaturedStar(const BasicStar &star, Feature feat) : BasicStar(star), feat(feat) {}
  
  Feature feat;
};

// Feature of the pattern of a star's surrounding
// It is essentially a 2D map of a star's surrounding area in polar space.
// The value a a pixel represents a star's luminance if there is one at
// this position or is zero otherwise.
// TODO: Feature

// Generate a binary mask that encircles stars in the image.
// Input image should has only one channel.
// If thres <= 0, returns the high-pass image before thresholding.
cv::Mat CreateStarMask(cv::Mat image, double thres = 0.1);


void DetectStars(const cv::Mat &image, const cv::Mat &mask,
                 StarIndex *star_index);

void SaveStarIndex(std::string filename, const StarIndex &star_index);

// Select the brightest stars and use the surrounding stars to make
// a translation/rotation-invariant feature for each of them.
std::vector<FeaturedStar> ExtractStarPatterns(
  const StarIndex &star_index, int max_num_ref_stars, double win_radius);

#endif

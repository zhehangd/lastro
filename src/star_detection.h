#ifndef ASTROTOOLS_STAR_DETECTION_H_
#define ASTROTOOLS_STAR_DETECTION_H_

#include <opencv2/opencv.hpp>

// This module focuses on finding stars in a typical astronomy picture.
// It expects there may be noise, nonuniform background, light pollution,
// or landscape foreground and tries to reduce their disturbance.

namespace lastro {

// Basic infomation of a detected star
struct BasicStar {
  int x, y; // Coordinates in the image
  double value; // Brightness of the star TODO: rgb?
};

typedef std::vector<BasicStar> StarList;

// Generate a binary mask that encircles stars in the image.
// Input image should has only one channel.
// If thres <= 0, returns the high-pass image before thresholding.
cv::Mat CreateStarMask(cv::Mat image, double thres = 0.1);

void DetectStarsFromMask(const cv::Mat &image, const cv::Mat &mask,
                         StarList *star_index);

void SaveStarList(std::string filename, const StarList &star_index);

void LoadStarList(std::string filename, StarList &star_list);

// Finds out stars within a certain distance range around a point
// Since the center does not have to be a star, we take xy coordinates
// instead of a star object. In certain cases we do use a star as the
// center, and we don't want that star to be included in the output.
// To achieve this purpose one can set min_radius to a small nonzero
// value to exclude the star at exactly the center position.
void FilterStarsByDistance(
  int x, int y, const StarList &stars_in, StarList &stars_out,
  double max_radius, double min_radius = 0);

}

#endif

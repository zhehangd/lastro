#ifndef ASTROTOOL_DWT2_H_
#define ASTROTOOL_DWT2_H_

#include <opencv2/opencv.hpp>

void DWT2HighPass(cv::Mat src, cv::Mat &dst, int level=7);

#endif

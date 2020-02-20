#include "dwt2.h"

#include <glog/logging.h>

#include "wavelib/wavelib.h"

namespace {
void DWT2HighPassGray(cv::Mat src, cv::Mat &dst, int level) {
  int rows = src.rows;
  int cols = src.cols;
  const char *name = "db2";
  const char *ext = "sym";
  
  wave_object obj = wave_init(name);
  wt2_object wt = wt2_init(obj, "dwt", rows, cols, level);
  strcpy(wt->ext, ext);
  
  // TODO: current code does not support discontinuous dst layout
  CHECK_EQ(src.channels(), 1);
  src.convertTo(dst, CV_64F);
  
  double *dst_p = reinterpret_cast<double*>(dst.data);
  
  double *wavecoeffs = dwt2(wt, dst_p);
  CHECK_NOTNULL(wavecoeffs);
  
  int ir, ic;
  char type[] = "A";
  double *cLL = getWT2Coeffs(wt, wavecoeffs, level, type, &ir, &ic);
  CHECK_NOTNULL(cLL);
  
  // Remove DC
  cv::Mat frame(cv::Size(ic, ir), CV_64FC1, (void*)cLL);
  frame.setTo(0);
  
  idwt2(wt, wavecoeffs, dst_p);
  wave_free(obj);
  wt2_free(wt);
}
}

void DWT2HighPass(cv::Mat src, cv::Mat &dst, int level) {
  int depth = src.depth();
  int num_channels = src.channels();
  cv::Mat cache;
  src.convertTo(cache, CV_64F);
  if (num_channels == 1) {
    DWT2HighPassGray(cache, cache, level);
  } else {
    std::vector<cv::Mat> channels;
    cv::split(cache, channels);
    for (int k = 0; k < num_channels; ++k) {
      DWT2HighPassGray(channels[k], channels[k], level);
    }
    cv::merge(channels, cache);
  }
  cache.convertTo(dst, depth);
}



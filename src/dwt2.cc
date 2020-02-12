#include "dwt2.h"

#include <glog/logging.h>

#include "wavelib/wavelib.h"

void DWT2HighPass(cv::Mat src, cv::Mat &dst, int level) {
  int rows = src.rows;
  int cols = src.cols;
  const char *name = "db2";
  const char *ext = "sym";
  
  wave_object obj = wave_init(name);
  wt2_object wt = wt2_init(obj, "dwt", rows, cols, level);
  strcpy(wt->ext, ext);
  
  // TODO Make sure src is continuous and double and is single channel image.
  
  src.convertTo(dst, CV_64FC1);
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

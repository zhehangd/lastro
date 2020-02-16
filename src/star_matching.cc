#include "star_matching.h"

#include <algorithm>
#include <cmath>

namespace lastro {

double Distance(const Feature &f1, const Feature &f2) {
  double dist = 0;
  double sum1 = 0, sum2 = 0;
  for (auto v : f1) {sum1 += v;}
  for (auto v : f2) {sum2 += v;}
  double den = sum1 + sum2;
  if (den == 0) {return 1;}
  for (int i = 0; i < RES_TOTAL; ++i) {dist += (double)std::abs(f1[i] - f2[i]);}
  dist /= sum1 + sum2; // is this reasonable?
  return dist;
}
  
std::vector<int> BruteForceMatch(const std::vector<Feature> &group1,
                                 const std::vector<Feature> &group2,
                                 double threshold) {
  int h = group1.size();
  int w = group2.size();
  std::vector<double> dist_lut(w * h);
  for (int r = 0; r < h; ++r)
    for (int c = 0; c < w; ++c)
      dist_lut[r * w + c] = Distance(group1[r], group2[c]);
  
  //for (int r = 0; r < h; ++r) {
  //  auto it = dist_lut.begin() + r * w;
  //  LOG(INFO) << fmt::format("{:<4.2}", fmt::join(it, it + w, " , "));
  //}
  std::vector<int> pool; // elements from group1 that have no match yet
  for (int r = 0; r < h; ++r) {pool.push_back(r);}
  std::vector<int> group1_to(h, -1);
  std::vector<int> group2_to(w, -1);
  int num_matches = std::min(w, h);
  int cur_matches = 0;
  while (num_matches != cur_matches) {
    // Select one unpaired candidate
    int r = pool.back();
    // Find the best unpaired partner 
    double min_dist = std::numeric_limits<double>::infinity();
    int min_c = -1; // argmax_c dist(r, c)
    for (int c = 0; c < w; ++c) {
      double dist = dist_lut[r * w + c];
      if (dist >= min_dist) {continue;}
      if (group2_to[c] >= 0) {
        double dist_rival = dist_lut[group2_to[c] * w + c];
        if (dist >= dist_rival) {continue;}
      }
      min_dist = dist;
      min_c = c;
    }
    // If such partner can not be found,
    // there is no hope for this candidate to match anymore.
    // So we simply kick it out.
    if (min_c < 0 || min_dist > threshold) {
      pool.pop_back();
      continue;
    }
    
    int r_rival = group2_to[min_c];
    group1_to[r] = min_c;
    group2_to[min_c] = r;
    // If the partner already has a match and will turn to this new candidate
    if (r_rival >= 0) {
      group1_to[r_rival] = -1;
      pool.back() = r_rival;
    } else {
      ++cur_matches;
      pool.pop_back();
    }
  }
  return group1_to;
}

Feature GenerateFeature(Coords pos, const StarList &star_list,
                        double max_radius) {
  auto w = RES_ANGLE;
  auto h = RES_LENGTH;
  float angle_delta = 2 * CV_PI / w;
  float dist_delta = (double)max_radius / h;
  
  //cv::Mat spectrum(dsize, CV_64F, cv::Scalar(0));
  //std::string name = fmt::format("spectrum_{:04}_{:04}", (int)src.x, (int)src.y);
  
  // Find the brightest star as the angle reference
  std::size_t max_val_idx = 0;
  double max_val = 0;
  
  for (std::size_t i = 0; i < star_list.size(); ++i) {
    const auto &star = star_list[i];
    if (star.value > max_val) {
      max_val_idx = i;
      max_val = star.value;
    }
  }

  auto A0_x = star_list[max_val_idx].pos.x;
  auto A0_y = star_list[max_val_idx].pos.y;
  double A0 = std::atan2(A0_y - pos.y, A0_x - pos.x);
  
  Feature feat = {};
  for (const auto &star : star_list) {
    double dx = star.pos.x - pos.x;
    double dy = star.pos.y - pos.y;

    // Angle
    double angle = std::atan2(dy, dx) - A0;
    angle = std::fmod(angle, 2 * CV_PI);
    if (angle < 0) {angle += 2 * CV_PI;}

    double angle_f, angle_i;
    angle_f = std::modf(angle / angle_delta, &angle_i);

    double dist = std::sqrt(dx * dx + dy * dy);
    if (dist > max_radius) {continue;}

    double dist_f, dist_i;
    dist_f = std::modf(dist / dist_delta, &dist_i);

    int r0 = (int)dist_i;
    int c0 = (int)angle_i;
    int r1 = (r0 + 1) % h;
    int c1 = (c0 + 1) % w;
    
    double val = 255.0;
    double val00 = val * (1 - angle_f) * (1 - dist_f);
    double val01 = val * angle_f * (1 - dist_f);
    double val10 = val * (1 - angle_f) * dist_f;
    double val11 = val * angle_f * dist_f;
    
    feat[r0 * w + c0] += val00;
    feat[r0 * w + c1] += val01;
    feat[r1 * w + c0] += val10;
    feat[r1 * w + c1] += val11;
  }
  return feat;
}

std::vector<Descriptor> MakeDescriptors(const StarList &star_list) {
  StarList keystar_list;
  std::vector<Descriptor> dscr_list;
  FilterStarsByBrightness(star_list, keystar_list, 20);
  for (std::size_t i = 0; i < keystar_list.size(); ++i) {
    const auto &keystar = keystar_list[i];
    StarList nearby_star_list;
    FilterStarsByDistance(star_list, keystar.pos, nearby_star_list, 200);
    dscr_list.emplace_back();
    auto &descr = dscr_list.back();
    descr.pos = keystar.pos;
    descr.feat = GenerateFeature(keystar.pos, nearby_star_list, 200);
  }
  return dscr_list;
}

std::vector<MatchPoint> MatchStar(const StarList &ref_star_list,
                                  const StarList &tar_star_list) {
  std::vector<MatchPoint> match_list;
  auto ref_descr_list = MakeDescriptors(ref_star_list);
  auto tar_descr_list = MakeDescriptors(tar_star_list);
  std::vector<Feature> ref_feat_list;
  for (auto &dscr : ref_descr_list) {ref_feat_list.push_back(dscr.feat);};
  std::vector<Feature> tar_feat_list;
  for (auto &dscr : tar_descr_list) {tar_feat_list.push_back(dscr.feat);};
  std::vector<int> id_map = BruteForceMatch(ref_feat_list, tar_feat_list);
  for (std::size_t i = 0; i < id_map.size(); ++i) {
    if (id_map[i] >= 0) {
      match_list.emplace_back();
      auto &pair = match_list.back();
      pair.a = ref_descr_list[i].pos;
      pair.b = tar_descr_list[i].pos;
    }
  }
  return match_list;
}

void DrawStarPattern(cv::Mat &canvas, int x, int y, const StarList &stars,
                     cv::Scalar color) {
  for (const auto &star : stars) {
    cv::line(canvas, {x, y}, {star.pos.x, star.pos.y}, color);
  }
}

}

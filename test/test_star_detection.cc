#include <gtest/gtest.h> 

#include "star_detection.h"

TEST(FilterStarsByBrightness, OutPlace) {
  lastro::StarList src {
    {0, 0, 4}, {0, 0, 10}, {0, 0, 8}, {0, 0, 2}, {0, 0, 1}, {0, 0, 9},};
  lastro::StarList dst {};
  lastro::FilterStarsByBrightness(src, dst, 2);
  ASSERT_EQ(dst.size(), 2);
  EXPECT_EQ(dst[0].value, 10);
  EXPECT_EQ(dst[1].value, 9);
}

TEST(FilterStarsByBrightness, InPlace) {
  lastro::StarList data {
    {0, 0, 4}, {0, 0, 10}, {0, 0, 8}, {0, 0, 2}, {0, 0, 1}, {0, 0, 9},};
  lastro::FilterStarsByBrightness(data, data, 2);
  ASSERT_EQ(data.size(), 2);
  EXPECT_EQ(data[0].value, 10);
  EXPECT_EQ(data[1].value, 9);
}

TEST(FilterStarsByBrightness, FewerStars) {
  lastro::StarList data {{0, 0, 4}, {0, 0, 10}, {0, 0, 8}};
  lastro::FilterStarsByBrightness(data, data, 5);
  ASSERT_EQ(data.size(), 3);
  EXPECT_EQ(data[0].value, 10);
  EXPECT_EQ(data[1].value, 8);
  EXPECT_EQ(data[2].value, 4);
}

TEST(FilterStarsByBrightness, EmptyList) {
  lastro::StarList src {};
  lastro::StarList dst {{}, {}};
  lastro::FilterStarsByBrightness(src, dst, 4);
  ASSERT_EQ(dst.size(), 0);
}

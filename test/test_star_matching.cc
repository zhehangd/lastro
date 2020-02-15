#include <gtest/gtest.h> 

#include "star_matching.h"

TEST(BruteForceMatch, Basic) {
  std::vector<lastro::Feature> f1s {
    {0, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 1}, 
  };
  
  std::vector<lastro::Feature> f2s {
    {0, 0, 1, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, 
  };
  
  auto mapping = lastro::BruteForceMatch(f1s, f2s);
  ASSERT_EQ(mapping.size(), 4);
  EXPECT_EQ(mapping[0], 0);
  EXPECT_EQ(mapping[1], 1);
  EXPECT_EQ(mapping[2], 3);
  EXPECT_EQ(mapping[3], 2);
}


TEST(BruteForceMatch, Src1HasMoreElements) {
  std::vector<lastro::Feature> f1s {
    {0, 0, 0, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 0, 1, 0}, {0, 1, 1, 1, 1},
    {1, 1, 1, 1, 1}
  };
  
  std::vector<lastro::Feature> f2s {
    {0, 0, 1, 0, 0}, {1, 0, 0, 0, 0}, {0, 1, 1, 1, 0}, {0, 1, 0, 1, 0}, 
  };
  
  auto mapping = lastro::BruteForceMatch(f1s, f2s);
  ASSERT_EQ(mapping.size(), 5);
  EXPECT_EQ(mapping[0], -1);
  EXPECT_EQ(mapping[1], 1);
  EXPECT_EQ(mapping[2], 3);
  EXPECT_EQ(mapping[3], 2);
  EXPECT_EQ(mapping[4], 0);
}
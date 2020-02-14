#include <gtest/gtest.h> 

// Making sure google test is working.
TEST(Test, GTestWorks) {
  EXPECT_EQ(6 * 7, 42);
}

int main(int argc, char* argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

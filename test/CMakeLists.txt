include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_all
  test_main.cc
  test_star_detection.cc
  test_star_matching.cc
)

target_link_libraries(test_all
  lastro_objs
  ${GTEST_BOTH_LIBRARIES}
  Threads::Threads
)

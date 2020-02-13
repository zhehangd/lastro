add_executable(lastro
  main.cc
  main_star_detection.cc
  
  dwt2.cc
  star_detection.cc
  star_matching.cc
  common.cc
)

target_link_libraries(lastro
  gflags
  glog
  CLI11::CLI11
  wavelib::libwavelib
  fmt
  ${OpenCV_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
)

# Data
# --------------------------------
execute_process (COMMAND ${CMAKE_COMMAND} -E create_symlink ../data data)

install(TARGETS lastro
  RUNTIME DESTINATION bin
)

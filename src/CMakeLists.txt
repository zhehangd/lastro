
add_library(lastro_objs OBJECT
  core.cc
  dwt2.cc
  star_detection.cc
  star_matching.cc
)

target_include_directories(lastro_objs PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(lastro_objs
  glog
  wavelib::libwavelib
  fmt
  ${OpenCV_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(lastro
  main.cc
  main_star_detection.cc
  main_star_matching.cc
  utilities.cc
  $<TARGET_OBJECTS:lastro_objs>
)

target_link_libraries(lastro
  lastro_objs
  glog
  CLI11::CLI11
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

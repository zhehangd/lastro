set(OBJECT_LIB_NAME "lastro_lib_objs")

add_library(${OBJECT_LIB_NAME} OBJECT
  dwt2.cc
  star_detection_and_matching.cc
  common.cc
)

target_link_libraries(${OBJECT_LIB_NAME}
  gflags
  glog
  wavelib::libwavelib
  fmt
  ${OpenCV_LIBS}
  ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(lastro
  main.cc
  main_starmask.cc
  main_starlist.cc
  $<TARGET_OBJECTS:lastro_lib_objs>
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

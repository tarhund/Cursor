# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/camera-viewer_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/camera-viewer_autogen.dir/ParseCache.txt"
  "camera-viewer_autogen"
  )
endif()

# Copyright (C) 2010 Aldebaran Robotics

cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
project(Prog)
# this lets you find the qibuild cmake framework
find_package(qibuild)

# Here, we set the varible PROG_IS_REMOTE to OFF
option(PROG_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  OFF)

# Here, we create an executable named "Prog" from the source files.
set(_srcs
    main.cpp
    configuration.hpp
    action.hpp
    camera.hpp
    camera.cpp
    gestures.hpp
    shapes.hpp
)

# Here we set the path to the static libs
set(LIB_DIR
  $ENV{PWD}/../lib
)

link_directories(${LIB_DIR})

if(PROG_IS_REMOTE)
  add_definitions(" -DPROG_IS_REMOTE ")
  qi_create_bin(Prog ${_srcs})
else()
  qi_create_lib(Prog SHARED ${_srcs} SUBFOLDER naoqi)
  target_link_libraries(Prog cvblob)
endif()

# Here we say that our executable depends on
# - ALCOMMON (main naoqi lib)
# - ALVISION (for vision definitions)
# - OPENCV (display)
#
# It automatically links with the corresponding libraries and make their headers available.
qi_use_lib(Prog ALCOMMON BOOST ALVISION OPENCV2_CORE OPENCV2_IMGPROC OPENCV2_HIGHGUI)




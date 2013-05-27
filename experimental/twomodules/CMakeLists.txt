# Copyright (C) 2010 Aldebaran Robotics

cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
project(twomodules)
find_package(qibuild)

option(TWOMODULES_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  OFF)

set(_srcs
    main.cpp
    RecordVid.h
    RecordVid.cpp
    Walk.h
    Walk.cpp
)

if(TWOMODULES_IS_REMOTE)
  add_definitions(" -DTWOMODULES_IS_REMOTE ")
  qi_create_bin(twomodules ${_srcs})
else()
  qi_create_lib(twomodules SHARED ${_srcs} SUBFOLDER naoqi)
endif()


qi_use_lib(twomodules ALCOMMON BOOST)

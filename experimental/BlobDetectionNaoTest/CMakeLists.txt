cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
project(BlobDetectionNao)
# this lets you find the qibuild cmake framework
find_package(qibuild)

# Here, we set the varible BLOBDETECTION_IS_REMOTE to OFF
option(BLOBDETECTION_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  OFF)

# Here, we create an executable named "FingerDetection" from the source files.
set(_srcs
    main.cpp
    BlobDetection.h
    BlobDetection.cpp
    ./cvblob/blob.cpp
    ./cvblob/blob.h
    ./cvblob/BlobContour.cpp
    ./cvblob/BlobContour.h
    ./cvblob/BlobLibraryConfiguration.h
    ./cvblob/BlobOperators.cpp
    ./cvblob/BlobOperators.h
    ./cvblob/BlobResult.cpp
    ./cvblob/BlobResult.h
    ./cvblob/ComponentLabeling.cpp
    ./cvblob/ComponentLabeling.h
)

# Here we set the path to the static libs
#set(LIB_DIR
#  $ENV{PWD}/lib
#)
# Add user library path to the link directories
#link_directories(${LIB_DIR})

if(BLOBDETECTION_IS_REMOTE)
  add_definitions(" -BLOBDETECTION_IS_REMOTE ")
  qi_create_bin(BlobDetection ${_srcs})
else()
  # local for NAO
  qi_create_lib(BlobDetection SHARED ${_srcs} SUBFOLDER naoqi)
  #target_link_libraries(BlobDetection cvblob)
endif()

# Here we say that our executable depends on
# - ALCOMMON (main naoqi lib)
# - ALVISION (for vision definitions)
# - OPENCV (display)
#
# It automatically links with the corresponding libraries and make their headers available.
qi_use_lib(BlobDetection ALCOMMON ALVISION OPENCV2_CORE OPENCV2_IMGPROC OPENCV2_HIGHGUI)




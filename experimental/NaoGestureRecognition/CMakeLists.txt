cmake_minimum_required(VERSION 2.6.4 FATAL_ERROR)
project(NaoGestureRecognition)
# this lets you find the qibuild cmake framework
find_package(qibuild)

# Here, we set the varible NAO_GESTURERECOGNITION_IS_REMOTE to OFF
option(NAO_GESTURERECOGNITION_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  OFF)

# Here, we create an executable named "NaoGestureRecognition" from the source files.
set(_srcs
    main.cpp
    Nao.h
    Nao.cpp
    NaoGestureRecognition.h
    NaoGestureRecognition.cpp
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

if(NAO_GESTURERECOGNITION_IS_REMOTE)
  add_definitions(" -DNAO_GESTURERECOGNITION_IS_REMOTE ")
  qi_create_bin(NaoGestureRecognition ${_srcs})
else()
  # local for NAO
  qi_create_lib(NaoGestureRecognition SHARED ${_srcs} SUBFOLDER naoqi)
endif()

# Here we say that our executable depends on
# - ALCOMMON (main naoqi lib)
# - ALVISION (for vision definitions)
# - OPENCV (display)
#
# It automatically links with the corresponding libraries and make their headers available.
qi_use_lib(NaoGestureRecognition ALCOMMON ALVISION OPENCV2_CORE OPENCV2_IMGPROC OPENCV2_HIGHGUI)




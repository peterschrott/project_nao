
#ifndef CAMERA_H
#define CAMERA_H

#include <opencv2/core/core.hpp>

// allgemeine includes

#ifdef PC
// aldebaran includes
#define GET_CAMERA_IMAGE getOpenCvImage();

#else
// opencv includes
#define GET_CAMERA_IMAGE getNaoImage();
#endif

cv::Mat getOpenCvImage();
cv::Mat getNaoImage();
bool unsubscribe() ;

#endif

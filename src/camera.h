
#ifndef CAMERA_H
#define CAMERA_H


// allgemeine includes

#ifdef PC
// aldebaran includes

#define GET_CAMERA_IMAGE getOpenCvImage();
#else

// opencv includes
#define GET_CAMERA_IMAGE getNaoImage();
#endif

Mat getOpenCvImage();
Mat getNaoImage();

#endif

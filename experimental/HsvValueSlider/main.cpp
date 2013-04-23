//##using C++ library##
#include <iostream>
using namespace std;
#include <stdlib.h>
//##using openCV library##
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "opencv/cxcore.h"
using namespace cv;
int main ( int argc, char **argv ) //--------------MAIN BODY---------------
{
VideoCapture webCam(0); // video source for webcam
webCam.set(CV_CAP_PROP_FRAME_WIDTH,640);
webCam.set(CV_CAP_PROP_FRAME_HEIGHT,480);
// slices matrcies that hold H,S and V
vector<Mat> slices;
// Cross Element for Erosion/Dilation
Mat cross = getStructuringElement(MORPH_CROSS, Size(5,5));
// create matrices to hold image
Mat camImage;		// raw image from webcam
Mat blurImage;		// blur image
Mat hsvImage;		// hsv image
Mat hue;			// hue channel
Mat hue1;			// Hue upper bound
Mat hue2;			// Hue lower bound
Mat hue3;			// hue color filtering
Mat sat;			// Sat channel
Mat sat1;			// Sat upper bound
Mat sat2;			// sat lower bound
Mat sat3;			// sat color filtering
Mat val;			// Val channel
Mat val1;			// Val upper bound
Mat val2;			// Val lower bound
Mat val3;			// Val color filtering
Mat erd;			// Erosion Image
Mat dia;			// dialate image
Mat HnS;			// sat and hue channel
Mat HSV;			// HSV color fiter detected

// slide bar values
int HuethresH =0,
HuethresL =0,
SatthresL =0,
SatthresH = 0,
ValthresL =0,
ValthresH = 0,
erosionCount = 1,
blurSize = 3;

// new window
cvNamedWindow("Color Tune",CV_WINDOW_NORMAL);

// make tune bar
cvCreateTrackbar( "Hue UpperT","Color Tune", &HuethresH, 255, 0 );
cvCreateTrackbar ("Hue LowerT","Color Tune", &HuethresL,255, 0);
cvCreateTrackbar( "Sat UpperT","Color Tune", &SatthresH, 255, 0 );
cvCreateTrackbar( "Sat LowerT","Color Tune", &SatthresL, 255, 0 );
cvCreateTrackbar( "Val UpperT","Color Tune", &ValthresH, 255, 0 );
cvCreateTrackbar( "Val LowerT","Color Tune", &ValthresL, 255, 0 );
cvCreateTrackbar ("EroTime","Color Tune", &erosionCount,15, 0);
cvCreateTrackbar ("BlurSize","Color Tune", &blurSize,15, 0);
// check blurSize bound
if(blurSize == 0)
blurSize = 1; //reset blurSize

// get and display webcam image
while(1)
{
 // get new image over and over from webcam
 webCam >> camImage;

 // check blurSize bound
if(blurSize == 0)
blurSize = 1; //reset blurSize

// blur image
//blur(camImage, blurImage, Size(11,11));
blur(camImage, blurImage, Size(blurSize,blurSize));

// conver raw image to hsv
cvtColor (camImage, hsvImage, CV_RGB2HSV);

// check blurSize bound
if(blurSize == 0)
blurSize = 1; //reset blurSize
blur(hsvImage, hsvImage, Size(blurSize,blurSize));
//blur(hsvImage, hsvImage, Size(5,5));

// split image to H,S and V images
split(hsvImage,slices);

slices[0].copyTo (hue); // get the hue channel
slices[1].copyTo(sat); // get the sat channel
slices[2].copyTo(val); // get the V channel
//apply threshold HUE upper/lower for color range
threshold (hue,hue1,HuethresL,255, CV_THRESH_BINARY); // get lower bound
threshold (hue, hue2,HuethresH,255, CV_THRESH_BINARY_INV); // get upper bound

hue3 = hue1 &hue2; // multiply 2 matrix to get the color range

// apply thresshold for Sat channel
threshold (sat,sat1,SatthresL,255, CV_THRESH_BINARY); // get lower bound
threshold (sat, sat2,SatthresH,255, CV_THRESH_BINARY_INV); // get upper bound
sat3 = sat1 & sat2; // multiply 2 matrix to get the color range

// apply thresshold for Val channel
threshold (val,val1,SatthresL,255, CV_THRESH_BINARY); // get lower bound
threshold (val, val2,SatthresH,255, CV_THRESH_BINARY_INV); // get upper bound
val3 = val1 & val2; // multiply 2 matrix to get the color range
// combine sat and hue filter together
HnS = sat3 & hue3;
// erode and dialation to reduce noise
erode(HnS,erd,cross,Point(-1,-1),erosionCount); // do erode
dilate(HnS,dia,cross,Point(-1,-1),erosionCount);// do dialate
// combine sat, val and hue filter together
HSV = sat3 & hue3 & val3;

// erode and dialation to reduce noise
erode(HSV,erd,cross,Point(-1,-1),erosionCount); // do erode
dilate(HSV,dia,cross,Point(-1,-1),erosionCount); // do dialate

// display image over and over
imshow("Webcam Orignal", camImage);
//imshow("Blur", blurImage);
//imshow("HSV Image", hsvImage);
//imshow("Hue channel",hue);
//imshow("Lower bound",hue1);
//imshow("Upper bound", hue2);
//imshow("L channel", Val);
//imshow("Color detected", hue3);
//imshow("Erosion",erd);
imshow("Hue color",hue3);
//imshow("Sat channel",sat);
imshow("Sat color",sat3);
//imshow("Val color",val3);
imshow("Sat and Hue",HnS);
imshow("HSV",HSV);
// Pause for highgui to process image painting
cvWaitKey(5);
}
//------CLEAN UP-------
cvDestroyAllWindows();
}

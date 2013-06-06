#include "BlobDetection.h"

#include <iostream>

// opencv includes
#include <opencv2/opencv.hpp>

// Aldebaran includes
#include <alvision/alvideo.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <alvision/alimage.h>

// Include cvBlob
#include "cvblob/BlobResult.h"

using namespace cv;
using namespace std;
using namespace AL;

const int minBlobArea = 200;

BlobDetection::BlobDetection(boost::shared_ptr<ALBroker> broker, const std::string& name):
		ALModule(broker, name) {
	setModuleDescription("the module detects blobs and nao says the amout of fingers shown.");

	//functionName("Leds");
	//BIND_METHOD(ThreadTest::createThreads);
}

inline char klip( int val )
{
  if( val > 255 ) return (unsigned char)255;
  if( val < 0 )   return (unsigned char)0;
  return (unsigned char) val;
}

void convertYUV422ToBGR(int _width, int _height, int _nScaleFactor,
                        const unsigned char* _imageDataIn,
                        unsigned char* _imageDataOut)
{
  // integer implementation (4.9 times faster) - PEV
  unsigned char * ptuc_imageDataOut = (unsigned char *)_imageDataOut;
  const unsigned char * ptuc_imageDataIn = (const unsigned char *)_imageDataIn;

  int widthMax = _width*2*_nScaleFactor;
  int widthStep= 4*_nScaleFactor;
  for(int j=0; j < _height; j++)
  { for(int i=0; i < widthMax; i+=widthStep)
    { int C = ptuc_imageDataIn[i  ] - 16;
      int D = ptuc_imageDataIn[i+1] - 128;
      int C2= ptuc_imageDataIn[i+2] - 16;
      int E = ptuc_imageDataIn[i+3] - 128;

      register int valC1 = 298 * C;
      register int valC2 = 298 * C2;
      register int valB  = 517 * D;
      register int valG  = - 208 * D - 100 * E;
      register int valR  = 409 * E;

      *ptuc_imageDataOut++ = klip(( valC1 + valB ) >> 8);
      *ptuc_imageDataOut++ = klip(( valC1 + valG ) >> 8);
      *ptuc_imageDataOut++ = klip(( valC1 + valR ) >> 8);
      *ptuc_imageDataOut++ = klip(( valC2 + valB ) >> 8);
      *ptuc_imageDataOut++ = klip(( valC2 + valG ) >> 8);
      *ptuc_imageDataOut++ = klip(( valC2 + valR ) >> 8);
    }
    ptuc_imageDataIn += widthMax*_nScaleFactor;
  }
}

void BlobDetection::init() {
	/** Init is called just after construction.   */
	try {
		/** Create a proxy to ALVideoDevice on the robot.*/
		ALVideoDeviceProxy *camProxy = new ALVideoDeviceProxy(getParentBroker());
		/** Subscribe a client image requiring 640*480px and RGB colorspace.*/
		const std::string clientName = camProxy->subscribeCamera("camera_01", 0, AL::kVGA, AL::kRGBColorSpace , 10); //AL::kBGRColorSpace

		handOrientation rightOrientationLast = NONE, leftOrientationLast = NONE;
		handOrientation rightOrientationCur = NONE, leftOrientationCur = NONE;

		// prepare vido recording
		long long timestamp = 0;
	    std::string arvFile = std::string("/home/nao/video");

        streamHeader tmpStreamHeader;
        std::vector<streamHeader> streamHeaderVector;
        ALVideo videoFile;

        tmpStreamHeader.width      = 640;
        tmpStreamHeader.height     = 480;
        tmpStreamHeader.colorSpace = AL::kRGBColorSpace;
        tmpStreamHeader.pixelDepth = 8;

        streamHeaderVector.push_back(tmpStreamHeader);

        std::cout<<"Output arv file properties: "<< streamHeaderVector[0].width <<"x"<< streamHeaderVector[0].height
          <<" Colorspace id:"<< streamHeaderVector[0].colorSpace <<" Pixel depth:"<< streamHeaderVector[0].pixelDepth
          <<std::endl;

        if( !videoFile.recordVideo( arvFile, 0, streamHeaderVector ) ) {
            std::cout<<"Error writing "<< arvFile <<" file."<<std::endl;
            return;
        }

        int j = 0;
		while(j++ < 30) {
			ALImage *camImg = (ALImage*)camProxy->getImageLocal(clientName);

            timestamp += 66000;

			/** Access the image buffer (6th field) and assign it to the opencv image container. */
			/** Create an Mat header to wrap into an opencv image.*/
			Mat naoImg = Mat(Size(camImg->getWidth(), camImg->getHeight()), CV_8UC3);

			//convertYUV422ToBGR(camImg->getWidth(), camImg->getHeight(), 1, camImg->getData(), naoImg.data);
			//naoImg.data = (uchar*) camImg->getData();

			// Convert the image to HSV colors.
			cvtColor(naoImg, naoImg, CV_RGB2HSV);

			// Get the separate HSV color components of the color input image.
			std::vector<Mat> channels(3);
			split(naoImg, channels);

			Mat planeH = channels[0];
			Mat planeS = channels[1];
			Mat planeV = channels[2];

			// Detect which pixels in each of the H, S and V channels are probably skin pixels.
			threshold(planeH, planeH, 150, UCHAR_MAX, CV_THRESH_BINARY_INV);
			threshold(planeS, planeS, 60, UCHAR_MAX, CV_THRESH_BINARY);
			threshold(planeV, planeV, 170, UCHAR_MAX, CV_THRESH_BINARY);

			// Combine all 3 thresholded color components, so that an output pixel will only
			// be white if the H, S and V pixels were also white.
			Mat imageSkinPixels(naoImg.size(), CV_8UC1);
			bitwise_and(planeH, planeS, imageSkinPixels);
			bitwise_and(imageSkinPixels, planeV, imageSkinPixels);

			// Find blobs in the image.
			IplImage* ipl_imageSkinPixels = cvCreateImage(imageSkinPixels.size(), 8, 1);
			ipl_imageSkinPixels->imageData = (char *) imageSkinPixels.data;
			//IplImage ipl_imageSkinPixels = imageSkinPixels;

            videoFile.write((char*) ipl_imageSkinPixels->imageData, ipl_imageSkinPixels->imageSize);


			CBlobResult blobs;
			blobs.ClearBlobs();
			blobs = CBlobResult(ipl_imageSkinPixels, NULL, 0);	// Use a black background color.

			// Ignore the blobs whose area is less than minArea.
			blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minBlobArea);

			// ##### Gestures #####
			std::cout << "Number of Blobs: "<< blobs.GetNumBlobs() <<endl;
			if(blobs.GetNumBlobs() == 0){
				//picture empty
			}else if(blobs.GetNumBlobs() == 1){
				//head detected
			}else if(blobs.GetNumBlobs() == 2 || blobs.GetNumBlobs() == 3){
				//head + one hand || head + two hands
				Rect rect[3];
				int indexHead = -1, indexHandLeft = -1, indexHandRight = -1;

				//Get Bounding Boxes
				for(int i = 0; i< blobs.GetNumBlobs(); i++){
					rect[i] = blobs.GetBlob(i)->GetBoundingBox();
				}

				//Detect Head and Hand indexes
				if(blobs.GetNumBlobs() == 2){
					int indexHand = -1;
					if(getCenterPoint(rect[0]).y < getCenterPoint(rect[1]).y){
						indexHead = 0;
						indexHand = 1;
					}else{
						indexHead = 1;
						indexHand = 0;
					}

					if(getHandside(rect[indexHead], rect[indexHand]) == LEFT){
						indexHandLeft = 1;
						indexHandRight = -1;
					}else{
						indexHandLeft = -1;
						indexHandRight = 1;
					}

				}else{
					//two hands
					int indexHand1 = -1;
					int indexHand2 = -1;
					if(getCenterPoint(rect[0]).y < getCenterPoint(rect[1]).y && getCenterPoint(rect[0]).y < getCenterPoint(rect[2]).y){
						indexHead = 0;
						indexHand1 = 1;
						indexHand2 = 2;
					}else if(getCenterPoint(rect[1]).y < getCenterPoint(rect[0]).y && getCenterPoint(rect[1]).y < getCenterPoint(rect[2]).y){
						indexHead = 1;
						indexHand1 = 0;
						indexHand2 = 2;
					}else{
						indexHead = 2;
						indexHand1 = 0;
						indexHand2 = 1;
					}

					if(getHandside(rect[indexHead], rect[indexHand1]) == LEFT){
						indexHandLeft = indexHand1;
						indexHandRight = indexHand2;
					}else{
						indexHandLeft = indexHand2;
						indexHandRight = indexHand1;
					}
				}

				//Get Orientations from Hand rects
				leftOrientationCur = (indexHandLeft != -1)?getOrientationOfRect(rect[indexHandLeft]):NONE;
				rightOrientationCur = (indexHandRight != -1)?getOrientationOfRect(rect[indexHandRight]):NONE;

				//Check Change of Left hand
				switch(detectHandStateChange(leftOrientationLast, leftOrientationCur)){
					case PORTRAIT_TO_LANDSCAPE:
						handleGestures(LEFT_FLIP_DOWN);
						break;
					case LANDSCAPE_TO_PORTRAIT:
						handleGestures(LEFT_FLIP_UP);
						break;
					case NOCHANGE:
						// TODO
					default:
						break;
				}

				//Check Change of Right hand
				switch(detectHandStateChange(rightOrientationLast, rightOrientationCur)){
					case PORTRAIT_TO_LANDSCAPE:
						handleGestures(RIGHT_FLIP_DOWN);
						break;
					case LANDSCAPE_TO_PORTRAIT:
						handleGestures(RIGHT_FLIP_UP);
						break;
					case NOCHANGE:
						//TODO
					default:
						break;
				}
			}else if(blobs.GetNumBlobs() > 3){
				//too much information
				cout<<"too much information"<<endl;
			}

			leftOrientationLast = leftOrientationCur;
			rightOrientationLast = rightOrientationCur;

			// Free all the resources.
			camProxy->releaseImage(clientName);

			cvReleaseImage(&ipl_imageSkinPixels);

			sleep(1);
		}

		videoFile.closeVideo();

		camProxy->unsubscribe(clientName);

	} catch (const AL::ALError& e) {
		std::cerr << "Caught exception: " << e.what() << std::endl;
		return;
	}
	return;
}

Point BlobDetection::getCenterPoint(Rect rect) {
	return Point(rect.x + rect.width, rect.y + rect.height);
}

handOrientation BlobDetection::getOrientationOfRect(Rect rect) {
	if(rect.width > rect.height) {
		return LANDSCAPE;
	}
	if(rect.height > rect.width) {
		return PORTRAIT;
	}
	return NONE;
}

handside BlobDetection::getHandside(Rect head, Rect hand) {
	//Camera Picture is inversed
	if(getCenterPoint(head).x > getCenterPoint(hand).x){
		return RIGHT;
	}else{
		return LEFT;
	}
}

handOrientationChange BlobDetection::detectHandStateChange(handOrientation last, handOrientation current) {
	if(last == NONE) {
	    return NOCHANGE; //TODO: ???
	}
	if(current == NONE) {
	    return NOCHANGE; //TODO: ???
	}

	if(last == PORTRAIT && current == LANDSCAPE) {
	    return PORTRAIT_TO_LANDSCAPE;
	}
	if(last == LANDSCAPE && current == PORTRAIT) {
	    return LANDSCAPE_TO_PORTRAIT;
	}

	return NOCHANGE;

}

int BlobDetection::handleGestures(gestures doGesture) {
	switch(doGesture){
		case LEFT_FLIP_DOWN:
			cout<<"LEFT FLIP DOWN"<<endl;
			break;
		case LEFT_FLIP_UP:
			cout<<"LEFT FLIP UP"<<endl;
			break;
		case RIGHT_FLIP_DOWN:
			cout<<"RIGHT FLIP DOWN"<<endl;
			break;
		case RIGHT_FLIP_UP:
			cout<<"RIGHT FLIP UP"<<endl;
			break;
		default:
			break;
	}
}

BlobDetection::~BlobDetection(){}

#include "BlobDetection.h"

// opencv includes
#include <opencv2/opencv.hpp>

// Aldebaran includes
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <alvalue/alvalue.h>

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

void BlobDetection::init() {
	/** Init is called just after construction.   */
	try {
		//OLD: CvCapture* capture = cvCreateFileCapture( "/home/peter/nao/workspace/repo/project_nao/experimental/BlobDetection/recording_01.avi");

		/** Create a proxy to ALVideoDevice on the robot.*/
		ALVideoDeviceProxy *camProxy = new ALVideoDeviceProxy("nao.local", 9559);
		/** Subscribe a client image requiring 640*480px and RGB colorspace.*/
		const std::string clientName = camProxy->subscribeCamera("camera_01", 1, AL::kVGA, AL::kRGBColorSpace, 10);
		
		handOrientation rightOrientationLast = NONE, leftOrientationLast = NONE;
		handOrientation rightOrientationCur = NONE, leftOrientationCur = NONE;

		//cvNamedWindow("Input Image", CV_WINDOW_AUTOSIZE);
		//cvNamedWindow("Skin Pixels", CV_WINDOW_AUTOSIZE);
		//cvNamedWindow("Skin Blobs", CV_WINDOW_AUTOSIZE);

		while(1) {
			//OLD: IplImage *imageBGR =  cvQueryFrame(capture);
			//if(!imageBGR) {
			//	break;
			//}
			//cvShowImage("Input Image", imageBGR);
			
			ALValue rgbImg = camProxy->getImageLocal(clientName);

			/** Access the image buffer (6th field) and assign it to the opencv image container. */
			/** Create an Mat header to wrap into an opencv image.*/
			Mat naoImg = Mat(Size(rgbImg[0], rgbImg[1]), CV_8UC3);
			naoImg.data = (uchar*) rgbImg[6].GetBinary();
			
			// OLD: Convert the image to HSV colors.
			//IplImage *imageHSV = cvCreateImage( cvGetSize(imageBGR), 8, 3);	// Full HSV color image.
			//cvCvtColor(imageBGR, imageHSV, CV_BGR2HSV);				// Convert from a BGR to an HSV image
			
			/** Convert image from RGB to HSV */
			cvtColor(naoImg, naoImg, CV_RGB2HSV);

			// OLD:Get the separate HSV color components of the color input image.
			//IplImage* planeH = cvCreateImage( cvGetSize(imageBGR), 8, 1);	// Hue component.
			//IplImage* planeS = cvCreateImage( cvGetSize(imageBGR), 8, 1);	// Saturation component.
			//IplImage* planeV = cvCreateImage( cvGetSize(imageBGR), 8, 1);	// Brightness component.
			//cvCvtPixToPlane(imageHSV, planeH, planeS, planeV, 0);	// Extract the 3 color components.
			
			std::vector<Mat> channels(3);
			split(naoImg, channels);
			
			Mat planeH = channels[0];
			Mat planeS = channels[1];
			Mat planeV = channels[2];
			
			// OLD: Detect which pixels in each of the H, S and V channels are probably skin pixels.
			//cvThreshold(planeH, planeH, 150, UCHAR_MAX, CV_THRESH_BINARY_INV);//18
			//cvThreshold(planeS, planeS, 60, UCHAR_MAX, CV_THRESH_BINARY);//50
			//cvThreshold(planeV, planeV, 170, UCHAR_MAX, CV_THRESH_BINARY);//80
			
			threshold(planeH, planeH, 150, UCHAR_MAX, CV_THRESH_BINARY_INV);
			threshold(planeS, planeS, 60, UCHAR_MAX, CV_THRESH_BINARY);
			threshold(planeV, planeV, 170, UCHAR_MAX, CV_THRESH_BINARY);

			// OLD: Combine all 3 thresholded color components, so that an output pixel will only
			// be white if the H, S and V pixels were also white.
			//IplImage* imageSkinPixels = cvCreateImage( cvGetSize(imageBGR), 8, 1);	// Greyscale output image.
			//cvAnd(planeH, planeS, imageSkinPixels);				// imageSkin = H {BITWISE_AND} S.
			//cvAnd(imageSkinPixels, planeV, imageSkinPixels);	// imageSkin = H {BITWISE_AND} S {BITWISE_AND} V.
			
			Mat imageSkinPixels(naoImg.size(), CV_8UC1);
			bitwise_and(planeH, planeS, imageSkinPixels);
			bitwise_and(imageSkinPixels, planeV, imageSkinPixels);

			// OLD: Show the output image on the screen.
			//cvShowImage("Skin Pixels", imageSkinPixels);

			// Find blobs in the image.
			IplImage ipl_imageSkinPixels = imageSkinPixels;
			
			CBlobResult blobs;
			blobs = CBlobResult(&ipl_imageSkinPixels, NULL, 0);	// Use a black background color.

			// Ignore the blobs whose area is less than minArea.
			blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minBlobArea);
			//srand (time(NULL));

			// OLD: Show the large blobs.
			//IplImage* imageSkinBlobs = cvCreateImage( cvGetSize(imageBGR), 8, 3);	//Colored Output//,1); Greyscale output image.
			//for (int i = 0; i < blobs.GetNumBlobs(); i++) {
			//	CBlob *currentBlob = blobs.GetBlob(i);
			//	currentBlob->FillBlob(imageSkinBlobs, CV_RGB(rand()%255,rand()%255,rand()%255));	// Draw the large blobs as white.

			//	cvDrawRect(imageSkinBlobs,
			//		cvPoint(currentBlob->GetBoundingBox().x,currentBlob->GetBoundingBox().y),
			//		cvPoint(currentBlob->GetBoundingBox().x + currentBlob->GetBoundingBox().width,currentBlob->GetBoundingBox().y + currentBlob->GetBoundingBox().height),
			//		cvScalar(0,0,255),2);//Draw Bounding Boxes
			//}

			// OLD: cvShowImage("Skin Blobs", imageSkinBlobs);

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
			/*cvReleaseImage( &imageBGR );
			cvReleaseImage( &imageHSV );
			cvReleaseImage( &planeH );
			cvReleaseImage( &planeS );
			cvReleaseImage( &planeV );
			cvReleaseImage( &imageSkinPixels );
			cvReleaseImage( &imageSkinBlobs );*/

			//if ESC is pressed then exit loop
			//cvWaitKey(33);
		}
		//cvWaitKey(0);
		
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
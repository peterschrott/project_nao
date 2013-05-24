/* Use Blob Detection to Detect Human Skin. by Shervin Emami, Oct 2010 (http://www.shervinemami.co.cc/).
   (Make sure the file "cvblobslib.lib" (from the cvBlobsLib Library) is added to your project).
 */

#include <iostream>

// Include OpenCV
#include "opencv/cv.h"
#include "opencv/highgui.h"


// Include cvBlobsLib
#include "BlobResult.h"


enum handOrientation{NONE = 0,PORTRAIT = 1,LANDSCAPE = 2};
enum handOrientationChange{NOCHANGE = 0,PORTRAIT_TO_LANDSCAPE = 1,LANDSCAPE_TO_PORTRAIT = 2};
enum handside{LEFT = 0,RIGHT = 1};
enum gestures{LEFT_FLIP_DOWN = 0,LEFT_FLIP_UP = 1,RIGHT_FLIP_DOWN = 2,RIGHT_FLIP_UP = 3};

int minBlobArea = 200;

using namespace cv;
using namespace std;


CvPoint getCenterPoint(CvRect rect){
    return cvPoint(rect.x + rect.width, rect.y + rect.height);
}

handOrientation getOrientationOfRect(CvRect rect){
    if(rect.width > rect.height)return LANDSCAPE;
    if(rect.height > rect.width)return PORTRAIT;
    return NONE;
}

handside getHandside(CvRect head, CvRect hand){
    //Camera Picture is inversed
    if(getCenterPoint(head).x > getCenterPoint(hand).x){
        return RIGHT;
    }else{
        return LEFT;
    }
}

handOrientationChange detectHandStateChange(handOrientation last, handOrientation current){
    if(last == NONE)return NOCHANGE; //TODO: ???
    if(current == NONE)return NOCHANGE; //TODO: ???

    if(last == PORTRAIT && current == LANDSCAPE)return PORTRAIT_TO_LANDSCAPE;
    if(last == LANDSCAPE && current == PORTRAIT)return LANDSCAPE_TO_PORTRAIT;

    return NOCHANGE;

}

int handleGestures(gestures doGesture){
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
        default: break;
    }
}

int main(int argc, char *argv[])
{


    CvCapture* capture = cvCreateFileCapture( "recording_01.avi");



    handOrientation rightOrientationLast = NONE, leftOrientationLast = NONE;
    handOrientation rightOrientationCur = NONE, leftOrientationCur = NONE;


	cvNamedWindow("Input Image", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Skin Pixels", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Skin Blobs", CV_WINDOW_AUTOSIZE);

    while(1){
        IplImage *imageBGR =  cvQueryFrame(capture);
        if(!imageBGR)break;
        cvShowImage("Input Image", imageBGR);

        // Convert the image to HSV colors.
        IplImage *imageHSV = cvCreateImage( cvGetSize(imageBGR), 8, 3);	// Full HSV color image.
        cvCvtColor(imageBGR, imageHSV, CV_BGR2HSV);				// Convert from a BGR to an HSV image.

        // Get the separate HSV color components of the color input image.
        IplImage* planeH = cvCreateImage( cvGetSize(imageBGR), 8, 1);	// Hue component.
        IplImage* planeS = cvCreateImage( cvGetSize(imageBGR), 8, 1);	// Saturation component.
        IplImage* planeV = cvCreateImage( cvGetSize(imageBGR), 8, 1);	// Brightness component.
        cvCvtPixToPlane(imageHSV, planeH, planeS, planeV, 0);	// Extract the 3 color components.


        // Detect which pixels in each of the H, S and V channels are probably skin pixels.
        cvThreshold(planeH, planeH, 150, UCHAR_MAX, CV_THRESH_BINARY_INV);//18
        cvThreshold(planeS, planeS, 60, UCHAR_MAX, CV_THRESH_BINARY);//50
        cvThreshold(planeV, planeV, 170, UCHAR_MAX, CV_THRESH_BINARY);//80


        // Combine all 3 thresholded color components, so that an output pixel will only
        // be white if the H, S and V pixels were also white.
        IplImage* imageSkinPixels = cvCreateImage( cvGetSize(imageBGR), 8, 1);	// Greyscale output image.
        cvAnd(planeH, planeS, imageSkinPixels);				// imageSkin = H {BITWISE_AND} S.
        cvAnd(imageSkinPixels, planeV, imageSkinPixels);	// imageSkin = H {BITWISE_AND} S {BITWISE_AND} V.

        // Show the output image on the screen.

        cvShowImage("Skin Pixels", imageSkinPixels);


        // Find blobs in the image.
        CBlobResult blobs;

        blobs = CBlobResult(imageSkinPixels, NULL, 0);	// Use a black background color.

        // Ignore the blobs whose area is less than minArea.

        blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minBlobArea);

        srand (time(NULL));

        // Show the large blobs.
        IplImage* imageSkinBlobs = cvCreateImage( cvGetSize(imageBGR), 8, 3);	//Colored Output//,1); Greyscale output image.
        for (int i = 0; i < blobs.GetNumBlobs(); i++) {
            CBlob *currentBlob = blobs.GetBlob(i);
            currentBlob->FillBlob(imageSkinBlobs, CV_RGB(rand()%255,rand()%255,rand()%255));	// Draw the large blobs as white.

             cvDrawRect(imageSkinBlobs,
                  cvPoint(currentBlob->GetBoundingBox().x,currentBlob->GetBoundingBox().y),
                  cvPoint(currentBlob->GetBoundingBox().x + currentBlob->GetBoundingBox().width,currentBlob->GetBoundingBox().y + currentBlob->GetBoundingBox().height),
                  cvScalar(0,0,255),
                  2);//Draw Bounding Boxes

        }

        cvShowImage("Skin Blobs", imageSkinBlobs);

        //Gestures

        //std::cout << "Number of Blobs: "<< blobs.GetNumBlobs() <<endl;



        if(blobs.GetNumBlobs() == 0){
            //picture empty
        }else if(blobs.GetNumBlobs() == 1){
            //head detected
        }else if(blobs.GetNumBlobs() == 2 || blobs.GetNumBlobs() == 3){
            //head + one hand || head + two hands
            CvRect rect[3];
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
        cvWaitKey(33);
	}
	cvWaitKey(0);

	return 0;
}

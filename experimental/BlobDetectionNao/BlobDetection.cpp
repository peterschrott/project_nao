#include "BlobDetection.h"

#include <iostream>

// opencv includes
#include <opencv2/opencv.hpp>

// Aldebaran includes
#include <alvision/alvideo.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <alvision/alimage.h>
#include <alproxies/albehaviormanagerproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alledsproxy.h>

//Sensor includes
#include <alvalue/alvalue.h>
#include <alcommon/alproxy.h>
#include <alcommon/albroker.h>
#include <qi/log.hpp>
#include <althread/alcriticalsection.h>

// Include cvBlob
#include "cvblob/BlobResult.h"

// our includes
#include "../math/angles.h"

using namespace cv;
using namespace std;
using namespace AL;

const string ARM_RIGHT_UP = "liftuprightarm_1";
const string ARM_LEFT_UP = "liftupleftarm_1";
const string ARM_BOTH_UP = "liftuparmboth_1";
const string STAND = "StandUp";

const string FACE_LED_BLUE = "face_leds_blue";
const string FACE_LED_GREEN = "face_leds_green";
const string FACE_LED_RED = "face_leds_red";

std::vector<std::string> names_blue;
std::vector<std::string> names_red;
std::vector<std::string> names_green;

const int minBlobArea = 200;

int handStatus = BOTH_DOWN;
int statusMask[4][4];

int touched = 0;
int red_on = 0;
int green_on = 0;
int blue_on = 0;

ALBehaviorManagerProxy* behavoirProxy;
ALLedsProxy* ledProxy;

BlobDetection::BlobDetection(boost::shared_ptr<ALBroker> broker, const std::string& name):
    ALModule(broker, name)
{
    setModuleDescription("the module detects blobs and nao says the amout of fingers shown.");

    functionName("onFrontTactilTouched", getName(), "Method wich is called after front tactil is touched.");
    BIND_METHOD(BlobDetection::onFrontTactilTouched);

    functionName("onMiddleTactilTouched", getName(), "Method wich is called after middle tactil is touched.");
    BIND_METHOD(BlobDetection::onMiddleTactilTouched);
}

void BlobDetection::onFrontTactilTouched()
{
    touched = 1;
}

void BlobDetection::onMiddleTactilTouched()
{
    touched = 0;
}

void BlobDetection::init()
{
    /** Init is called just after construction. */
    try
    {
        initStatusMask();
        // Create a proxy to ALVideoDevice on the robot.
        ALVideoDeviceProxy* camProxy = new ALVideoDeviceProxy(getParentBroker());
        behavoirProxy = new ALBehaviorManagerProxy(getParentBroker());
        ledProxy = new ALLedsProxy(getParentBroker());

        initLeds();

        // Subscribe a client image requiring 640*480px and RGB colorspace.
        const std::string cameraID = camProxy->subscribeCamera("camera_01", 0, AL::kVGA, AL::kRGBColorSpace , 10);

        // Create a proxy to ALMemoryProxy on the robot.
        ALMemoryProxy fMemoryProxy = ALMemoryProxy(getParentBroker());
        fMemoryProxy.subscribeToEvent("FrontTactilTouched", "BlobDetection","onFrontTactilTouched");
        fMemoryProxy.subscribeToEvent("MiddleTactilTouched", "BlobDetection","onMiddleTactilTouched");

        HandOrientation rightOrientationLast = NONE;
        HandOrientation leftOrientationLast = NONE;
        HandOrientation rightOrientationCur = NONE, leftOrientationCur = NONE;

        // stand up
        behavoirProxy->runBehavior(STAND);

        while(1)
        {
            if(touched)
            {
                //Switch LEDs RED OFF, BLUE ON
                if(red_on == 1)
                {
                    ledProxy->off(FACE_LED_RED);
                    red_on = 0;
                }
                if(blue_on == 0)
                {
                    ledProxy->on(FACE_LED_BLUE);
                    blue_on = 1;
                }
                // Fetch the image from the nao camera, we subscribed on. Its in RGB colorspace
                ALImage *img_cam = (ALImage*)camProxy->getImageLocal(cameraID);

                // Create an openCv Mat header to convert the aldebaran AlImage image.
                // To convert the aldebaran image only the data are of it are assigned to the openCv image.
                Mat img_hsv = Mat(Size(img_cam->getWidth(), img_cam->getHeight()), CV_8UC3);
                img_hsv.data = (uchar*) img_cam->getData();

                // Convert the RGB image from the camera to an HSV image */
                cvtColor(img_hsv, img_hsv, CV_RGB2HSV);

                // RECORDING: record converted to hsv video
                //videoFile.write((char*) img_hsv.data, size); //video ging hier

                // Get the separate HSV color components of the color input image.
                std::vector<Mat> channels(3);
                split(img_hsv, channels);

                Mat planeH = channels[0];
                Mat planeS = channels[1];
                Mat planeV = channels[2];

                // Detect which pixels in each of the H, S and V channels are probably skin pixels.
                threshold(planeH, planeH, 150, UCHAR_MAX, CV_THRESH_BINARY_INV);//18
                threshold(planeS, planeS, 60, UCHAR_MAX, CV_THRESH_BINARY);//50
                threshold(planeV, planeV, 170, UCHAR_MAX, CV_THRESH_BINARY);//80

                // Combine all 3 thresholded color components, so that an output pixel will only
                // be white if the H, S and V pixels were also white.
                Mat imageSkinPixels = Mat(img_hsv.size(), CV_8UC3);	// Greyscale output image.
                bitwise_and(planeH, planeS, imageSkinPixels);	// imageSkin = H {BITWISE_AND} S.
                bitwise_and(imageSkinPixels, planeV, imageSkinPixels);	// imageSkin = H {BITWISE_AND} S {BITWISE_AND} V.

                // Assing the Mat (C++) to an IplImage (C), this is necessary because the blob detection is writtn in old opnCv C version
                IplImage ipl_imageSkinPixels = imageSkinPixels;

                // Set up the blob detection.
                CBlobResult blobs;
                blobs.ClearBlobs();
                blobs = CBlobResult(&ipl_imageSkinPixels, NULL, 0);	// Use a black background color.

                // Ignore the blobs whose area is less than minArea.
                blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minBlobArea);

                // ##### Gestures #####
                std::cout << "Number of Blobs: " << blobs.GetNumBlobs() <<endl;
                if(blobs.GetNumBlobs() == 0)
                {
                    //picture empty
                }
                else if(blobs.GetNumBlobs() == 1)
                {
                    //head detected
                }
                else if(blobs.GetNumBlobs() == 2 || blobs.GetNumBlobs() == 3)
                {
                    //head + one hand || head + two hands
                    Rect rect[3];
                    int indexHead = -1, indexHandLeft = -1, indexHandRight = -1;

                    //Get Bounding Boxes
                    for(int i = 0; i< blobs.GetNumBlobs(); i++)
                    {
                        rect[i] = blobs.GetBlob(i)->GetBoundingBox();
                    }

                    //Detect Head and Hand indexes
                    if(blobs.GetNumBlobs() == 2)
                    {
                        // head and one hand
                        int indexHand = -1;
                        if(getCenterPoint(rect[0]).y < getCenterPoint(rect[1]).y)
                        {
                            // rect[0] is head
                            indexHead = 0;
                            indexHand = 1;
                        }
                        else
                        {
                            // rect[1] is head
                            indexHead = 1;
                            indexHand = 0;
                        }

                        if(getHandside(rect[indexHead], rect[indexHand]) == LEFT)
                        {
                            // hand is left
                            indexHandLeft = 1;
                            indexHandRight = -1;
                        }
                        else
                        {
                            // hand ist right
                            indexHandLeft = -1;
                            indexHandRight = 1;
                        }

                    }
                    else
                    {
                        //two hands
                        int indexHand1 = -1;
                        int indexHand2 = -1;
                        if(getCenterPoint(rect[0]).y < getCenterPoint(rect[1]).y && getCenterPoint(rect[0]).y < getCenterPoint(rect[2]).y)
                        {
                            // rect[0] is head
                            indexHead = 0;
                            indexHand1 = 1;
                            indexHand2 = 2;
                        }
                        else if(getCenterPoint(rect[1]).y < getCenterPoint(rect[0]).y && getCenterPoint(rect[1]).y < getCenterPoint(rect[2]).y)
                        {
                            // rect[1] is head
                            indexHead = 1;
                            indexHand1 = 0;
                            indexHand2 = 2;
                        }
                        else
                        {
                            // rect[2] is head
                            indexHead = 2;
                            indexHand1 = 0;
                            indexHand2 = 1;
                        }

                        if(getHandside(rect[indexHead], rect[indexHand1]) == LEFT)
                        {
                            indexHandLeft = indexHand1;
                            indexHandRight = indexHand2;
                        }
                        else
                        {
                            indexHandLeft = indexHand2;
                            indexHandRight = indexHand1;
                        }
                    }

                    // bobs are detected.
                    // adjuste naos head to detected head-bolb
                    trackHead(getCenterPoint(rect[indexHead]).x, getCenterPoint(rect[indexHead]).y);

                    //Get Orientations from Hand rects
                    leftOrientationCur = (indexHandLeft != -1)?getOrientationOfRect(rect[indexHandLeft]):NONE;
                    rightOrientationCur = (indexHandRight != -1)?getOrientationOfRect(rect[indexHandRight]):NONE;

                    //Check Change of Left hand
                    switch(detectHandStateChange(leftOrientationLast, leftOrientationCur))
                    {
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
                    switch(detectHandStateChange(rightOrientationLast, rightOrientationCur))
                    {
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
                }
                else if(blobs.GetNumBlobs() > 3)
                {
                    //too much information
                    cout<<"too much information"<<endl;
                }

                leftOrientationLast = leftOrientationCur;
                rightOrientationLast = rightOrientationCur;

                // Free all the resources.
                camProxy->releaseImage(cameraID);

                //IplImage* p_iplImage = &ipl_imageSkinPixels;
                //cvReleaseImage(&p_iplImage);

                sleep(1);
            }
            else
            {
                //Switch LEDs RED ON, BLUE OFF
                if(red_on == 0)
                {
                    ledProxy->on(FACE_LED_RED);
                    red_on = 1;
                }
                if(blue_on == 1)
                {
                    ledProxy->off(FACE_LED_BLUE);
                    blue_on = 0;
                }
                behavoirProxy->runBehavior(STAND);
            }
        }
        camProxy->unsubscribe(cameraID);

    }
    catch (const AL::ALError& e)
    {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return;
    }
    return;
}

Point BlobDetection::getCenterPoint(Rect rect)
{
    return Point(rect.x + rect.width, rect.y + rect.height);
}

HandOrientation BlobDetection::getOrientationOfRect(Rect rect)
{
    if(rect.width > rect.height)
    {
        return LANDSCAPE;
    }
    if(rect.height > rect.width)
    {
        return PORTRAIT;
    }
    return NONE;
}

Handside BlobDetection::getHandside(Rect head, Rect hand)
{
//Camera Picture is inversed
    if(getCenterPoint(head).x > getCenterPoint(hand).x)
    {
        return RIGHT;
    }
    else
    {
        return LEFT;
    }
}

HandOrientationChange BlobDetection::detectHandStateChange(HandOrientation last, HandOrientation current)
{
    if(last == NONE)
    {
        return NOCHANGE; //TODO: ???
    }
    if(current == NONE)
    {
        return NOCHANGE; //TODO: ???
    }

    if(last == PORTRAIT && current == LANDSCAPE)
    {
        return PORTRAIT_TO_LANDSCAPE;
    }
    if(last == LANDSCAPE && current == PORTRAIT)
    {
        return LANDSCAPE_TO_PORTRAIT;
    }

    return NOCHANGE;

}

int BlobDetection::handleGestures(Gesture doGesture)
{
    switch(doGesture)
    {
    case LEFT_FLIP_DOWN:
        cout<<"LEFT FLIP DOWN"<<endl;
        updateStatus(LEFT_FLIP_DOWN);
        //behavoirProxy->runBehavior(STAND);
        break;
    case LEFT_FLIP_UP:
        cout<<"LEFT FLIP UP"<<endl;
        updateStatus(LEFT_FLIP_UP);
        //behavoirProxy->runBehavior(ARM_RIGHT_UP);
        break;
    case RIGHT_FLIP_DOWN:
        cout<<"RIGHT FLIP DOWN"<<endl;
        updateStatus(RIGHT_FLIP_DOWN);
        //behavoirProxy->runBehavior(STAND);
        break;
    case RIGHT_FLIP_UP:
        cout<<"RIGHT FLIP UP"<<endl;
        updateStatus(RIGHT_FLIP_UP);
        //behavoirProxy->runBehavior(ARM_LEFT_UP);
        break;
    default:
        break;
    }
}

int BlobDetection::updateStatus(Gesture gesture)
{
    //Set new Status
    /*switch(gesture)
    {
    case LEFT_FLIP_UP:
        handStatus = handStatus | (UP<<1);
        break;
    case RIGHT_FLIP_UP:
        handStatus = handStatus | (UP);
        break;
    case LEFT_FLIP_DOWN:
        handStatus = handStatus & ((DOWN << 1)^1);
        break;
    case RIGHT_FLIP_DOWN:
        handStatus = handStatus & (DOWN ^ 2 );
        break;
    }*/

    handStatus = statusMask[gesture][handStatus];

    //call Behaviors
    switch(handStatus)
    {
    case LEFT_UP_RIGHT_DOWN:
        behavoirProxy->runBehavior(ARM_RIGHT_UP);
        break;
    case LEFT_DOWN_RIGHT_UP:
        behavoirProxy->runBehavior(ARM_LEFT_UP);
        break;
    case BOTH_UP:
        behavoirProxy->runBehavior(ARM_BOTH_UP);
        break;
    case BOTH_DOWN:
        behavoirProxy->runBehavior(STAND);
        break;
    default:
        break;

    }
}

void BlobDetection::initStatusMask()
{
    //new Status = statusMask[gesture][oldStatus]
    statusMask[LEFT_FLIP_DOWN][LEFT_UP_RIGHT_DOWN]  = BOTH_DOWN;
    statusMask[LEFT_FLIP_DOWN][LEFT_DOWN_RIGHT_UP]  = LEFT_DOWN_RIGHT_UP;
    statusMask[LEFT_FLIP_DOWN][BOTH_DOWN]           = BOTH_DOWN;
    statusMask[LEFT_FLIP_DOWN][BOTH_UP]             = LEFT_DOWN_RIGHT_UP;

    statusMask[LEFT_FLIP_UP][LEFT_UP_RIGHT_DOWN]    = LEFT_UP_RIGHT_DOWN;
    statusMask[LEFT_FLIP_UP][LEFT_DOWN_RIGHT_UP]    = BOTH_UP;
    statusMask[LEFT_FLIP_UP][BOTH_DOWN]             = LEFT_UP_RIGHT_DOWN;
    statusMask[LEFT_FLIP_UP][BOTH_UP]               = BOTH_UP;

    statusMask[RIGHT_FLIP_DOWN][LEFT_UP_RIGHT_DOWN] = LEFT_UP_RIGHT_DOWN;
    statusMask[RIGHT_FLIP_DOWN][LEFT_DOWN_RIGHT_UP] = BOTH_DOWN;
    statusMask[RIGHT_FLIP_DOWN][BOTH_DOWN]          = BOTH_DOWN;
    statusMask[RIGHT_FLIP_DOWN][BOTH_UP]            = LEFT_UP_RIGHT_DOWN;

    statusMask[RIGHT_FLIP_UP][LEFT_UP_RIGHT_DOWN]   = BOTH_UP;
    statusMask[RIGHT_FLIP_UP][LEFT_DOWN_RIGHT_UP]   = LEFT_DOWN_RIGHT_UP;
    statusMask[RIGHT_FLIP_UP][BOTH_DOWN]            = LEFT_DOWN_RIGHT_UP;
    statusMask[RIGHT_FLIP_UP][BOTH_UP]              = BOTH_UP;
}

void BlobDetection::initLeds()
{
    // Create red led group FACE_LED_RED
    names_red.push_back("Face/Led/Red/Left/0Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/45Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/90Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/135Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/180Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/225Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/270Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/315Deg/Actuator/Value");

    names_red.push_back("Face/Led/Red/Right/0Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/45Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/90Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/135Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/180Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/225Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/270Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/315Deg/Actuator/Value");

    // Create green led group FACE_LED_GREEN
    names_green.push_back("Face/Led/Green/Left/0Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/45Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/90Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/135Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/180Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/225Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/270Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/315Deg/Actuator/Value");

    names_green.push_back("Face/Led/Green/Right/0Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/45Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/90Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/135Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/180Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/225Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/270Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/315Deg/Actuator/Value");

    //Create blue led group FACE_LED_BLUE
    names_blue.push_back("Face/Led/Blue/Left/0Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/45Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/90Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/135Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/180Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/225Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/270Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/315Deg/Actuator/Value");

    names_blue.push_back("Face/Led/Blue/Right/0Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/45Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/90Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/135Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/180Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/225Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/270Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/315Deg/Actuator/Value");

    ledProxy->createGroup(FACE_LED_GREEN,names_green);
    ledProxy->createGroup(FACE_LED_RED,names_red);
    ledProxy->createGroup(FACE_LED_BLUE,names_blue);

    ledProxy->setIntensity(FACE_LED_BLUE,1);
    ledProxy->setIntensity(FACE_LED_RED,1);
    ledProxy->setIntensity(FACE_LED_GREEN,1);

    ledProxy->off(FACE_LED_RED);
    ledProxy->off(FACE_LED_BLUE);
    ledProxy->off(FACE_LED_GREEN);
}

/**
*/
void BlobDetection::trackHead(int x, int y)
{

    // x value
    pixelToRad(x, 640, degreeToRad(60.9));


}

BlobDetection::~BlobDetection() {}

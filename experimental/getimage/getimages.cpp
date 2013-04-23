/**
 *
 * This example demonstrates how to get images from the robot remotely and how
 * to display them on your screen using opencv.
 *
 * Copyright Aldebaran Robotics
 */

// Aldebaran includes.
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>
#include <alerror/alerror.h>

// Opencv includes.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
#include <string>

using namespace AL;
using namespace cv;


/**
* \brief Shows images retrieved from the robot.
*
* \param robotIp the IP adress of the robot
*/
int captureImages(const std::string& robotIp) {

    /** determine the frames per second **/
    double fps = 30;

    /** Create a proxy to ALVideoDevice on the robot.*/
    ALVideoDeviceProxy camProxy(robotIp, 9559);

    /** Subscribe a client image requiring 1280*960px and YUV422 colorspace.*/
    const std::string clientName = camProxy.subscribeCamera("testCam", 1, k4VGA, kYUV422ColorSpace, fps);

    /** Create an Mat header to wrap into an opencv image.*/
    Mat imgHeader = Mat(Size(1280, 960), CV_8UC3);

    /** Create a OpenCV window to display the images. */
    namedWindow("images");

    /** set up the video writer */
    VideoWriter vWriter("vid_01.avi", CV_FOURCC('D','I','V','X'), fps, Size(1280, 960), true);

    if( !vWriter.isOpened() ) {
        printf("VideoWriter failed to open!\n");
        return -1;
    }

    /** Main loop. Exit when pressing ESC.*/
    while ((char) waitKey(30) != 27) {
        /** Retrieve an image from the camera.
        * The image is returned in the form of a container object, with the
        * following fields:
        * 0 = width
        * 1 = height
        * 2 = number of layers
        * 3 = colors space index (see alvisiondefinitions.h)
        * 4 = time stamp (seconds)
        * 5 = time stamp (micro seconds)
        * 6 = image buffer (size of width * height * number of layers)
        */
        ALValue img = camProxy.getImageRemote(clientName);

        /** Access the image buffer (6th field) and assign it to the opencv image
        * container. */
        imgHeader.data = (uchar*) img[6].GetBinary();

        /** write the image to the video writer **/
        vWriter << imgHeader;

        /** Tells to ALVideoDevice that it can give back the image buffer to the
        * driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
        camProxy.releaseImage(clientName);


        /** Display the iplImage on screen.*/
        imshow("images", imgHeader);
    }

    /** Cleanup.*/
    camProxy.unsubscribe(clientName);

    return 0;
}

int main(int argc, char* argv[]) {
    const std::string robotIp("192.168.1.111");

    try {
        int retVal = captureImages(robotIp);
        if(retVal != 0) {
            std::cerr << "captureImages() returned : " << retVal << std::endl;
        }
    } catch (const AL::ALError& e) {
        std::cerr << "Caught exception " << e.what() << std::endl;
    }

    return 0;
}

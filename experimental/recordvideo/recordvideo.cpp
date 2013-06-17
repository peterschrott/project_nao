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

// C++ includes.
#include <iostream>
#include <fstream>
#include <string>

using namespace AL;
using namespace cv;


/**
* \brief Shows images retrieved from the robot.
*
* \param robotIp the IP adress of the robot
*/
int captureVideo(const std::string& robotIp) {

    /** Create a proxy to ALVideoDevice on the robot.*/
    ALVideoDeviceProxy camProxy(robotIp, 9559);

    int width = 1280; int height = 960;
    int colorSpace = AL::kYUV422ColorSpace;
    int resolution = AL::k4VGA;

    std::string file = std::string("/home/peter/nao/video_");
    file += time(0) + ".avi";
    int fourcc = CV_FOURCC('M','J','P','G');
    int fps = 30;

    /** Create the output file */
    //std::ofstream outfile(file.c_str());
    //outfile.close();

    /** Subscribe a client image requiring 1280*960px and YUV422 colorspace.*/
    const std::string clientName = camProxy.subscribeCamera("camera_01", 1, resolution, colorSpace, fps);

    /** Create an Mat header to wrap into an opencv image.*/
    Mat cvImg = Mat(cv::Size(width, height), CV_8UC3);
    ALValue arvImg;

    /** set up the video writer */
    VideoWriter writer = VideoWriter(file, fourcc, fps, cv::Size(width, height), true);
    if (!writer.isOpened()) {
        std::cout<<"Error when creating writer."<<std::endl;
        return -5;
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
        arvImg = camProxy.getImageRemote(clientName);

        /** Access the image buffer (6th field) and assign it to the opencv image container. */
        cvImg.data = (uchar*) arvImg[6].GetBinary();

        /** write the image to the video writer **/
        writer << cvImg;

        /** Tells to ALVideoDevice that it can give back the image buffer to the
        * driver. Optional after a getImageRemote but MANDATORY after a getImageLocal.*/
        camProxy.releaseImage(clientName);

    }

    /** Cleanup.*/
    camProxy.unsubscribe(clientName);

    return 0;
}

int main(int argc, char* argv[]) {
    const std::string robotIp("192.168.0.111");

    try {
        int retVal = captureVideo(robotIp);
        if(retVal != 0) {
            std::cerr << "captureVideo() returned : " << retVal << std::endl;
        }
    } catch (const AL::ALError& e) {
        std::cerr << "Caught exception " << e.what() << std::endl;
    }

    return 0;
}

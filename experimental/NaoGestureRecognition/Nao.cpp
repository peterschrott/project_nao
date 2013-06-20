#include "Nao.h"

#include <alvision/alimage.h>

// constants for the led groups
const string FACE_LED_BLUE = "face_leds_blue";
const string FACE_LED_GREEN = "face_leds_green";
const string FACE_LED_RED = "face_leds_red";

/** @brief Nao
  *
  * consturctor
  */
Nao::Nao(boost::shared_ptr<AL::ALBroker> parentBroker)
:parentBroker(parentBroker) {
    initProxies();
    initLeds();
}

/** @brief dBehaviour
  *
  * @todo: document this function
  */
void Nao::doBehaviour(string behaviour) {

}

/** @brief getCameraImage
  *
  * Function gets a image from naos head camera and returns it as opencv Mat.
  */
Mat Nao::getCameraImage() {
    //nao camera, we subscribed on. Its in RGB colorspace
    ALImage *img_cam = (ALImage*)camProxy.getImageLocal(cameraId);

    // Create an openCv Mat header to convert the aldebaran AlImage image.
    // To convert the aldebaran image only the data are of it are assigned to the openCv image.
    Mat img_nao = Mat(Size(img_cam->getWidth(), img_cam->getHeight()), CV_8UC3);
    uchar* img_data = (uchar*)malloc(img_cam->getSize());
    memcpy(img_data, (uchar*)img_cam->getData(), img_cam->getSize());
    img_nao.data = img_data;

    // release the camara imge
    camProxy.releaseImage(cameraId);

    return img_nao;
}

/** @brief onFrontTactilTouched
  *
  * The callback function the front tractil.
  * This function is called event-driven,
  * if the front tractil on the naos head is touched.
  */
void Nao::onFrontTactilTouched() {
    touched = 1;
}

/** @brief onMiddleTactilTouched
  *
  * The callback function the middle tractil.
  * This function is called event-driven,
  * if the middle tractil on the naos head is touched.
  */
void Nao::onMiddleTactilTouched() {
    touched = 0;
}

/** @brief initLeds
  *
  * Initialze the led groups.
  */
void Nao::initLeds() {
    // Create a vector to access all RED leds
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

    // Create a vector to access all GREEN leds
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

    //Create a vector to access all BLUE leds
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

    // create the led groups
    ledProxy.createGroup(FACE_LED_GREEN, names_green);
    ledProxy.createGroup(FACE_LED_RED, names_red);
    ledProxy.createGroup(FACE_LED_BLUE, names_blue);

    // set the intensity of all led groups to maximum
    ledProxy.setIntensity(FACE_LED_BLUE, 1);
    ledProxy.setIntensity(FACE_LED_RED, 1);
    ledProxy.setIntensity(FACE_LED_GREEN, 1);

    // initiall shwich off all leds
    ledProxy.off(FACE_LED_RED);
    ledProxy.off(FACE_LED_BLUE);
    ledProxy.off(FACE_LED_GREEN);

    // set the led status
    red_on = 0;
    green_on = 0;
    blue_on = 0;
}

/** @brief initProxies
  *
  * Function to set up all neccessary Proxies.
  */
void Nao::initProxies() {
    // initialze all neccessary proxies
    camProxy = ALVideoDeviceProxy(parentBroker);
    behaviourProxy = ALBehaviorManagerProxy(parentBroker);
    ledProxy = ALLedsProxy(parentBroker);
    memProxy = ALMemoryProxy(parentBroker);
}

/** @brief ~Nao
  *
  * @todo: document this function
  */
 Nao::~Nao()
{

}

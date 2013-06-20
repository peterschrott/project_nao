#include "Nao.h"

#include <alvision/alimage.h>

// constants for the led groups
const string EYE_LEDS_BLUE = "eye_leds_blue";
const string EYE_LEDS_GREEN = "eye_leds_green";
const string EYE_LEDS_RED = "eye_leds_red";

// constens for the behaviour
const std::string ARM_RIGHT_UP = "liftuprightarm_1";
const std::string ARM_LEFT_UP = "liftupleftarm_1";
const std::string ARM_BOTH_UP = "liftuparmboth_1";
const std::string STAND = "StandUp";

/** @brief Nao
  *
  * consturctor
  */
Nao::Nao() {};

/** @brief Nao
  *
  * consturctor
  */
Nao::Nao(boost::shared_ptr<AL::ALBroker> parentBroker)
    :parentBroker(parentBroker)
{
    initProxies();
    initLeds();
    initFsm();

    // Subscribe a client image requiring 640*480px and RGB colorspace.
    //cameraId = camProxy->subscribeCamera("camera_01", 0, AL::kVGA, AL::kRGBColorSpace , 10);


    memProxy->subscribeToEvent("FrontTactilTouched", "NaoGestureRecognition","onFrontTactilTouched");
    memProxy->subscribeToEvent("MiddleTactilTouched", "NaoGestureRecognition","onMiddleTactilTouched");
}

/** @brief dBehaviour
  *
  * @todo: document this function
  */
void Nao::doBehaviour( string behaviour )
{
    // run the given behaviour
    behaviourProxy->runBehavior(behaviour);
}

/** @brief getCameraImage
  *
  * Function gets a image from naos head camera and returns it as opencv Mat.
  */
Mat Nao::getCameraImage()
{
    //nao camera, we subscribed on. Its in RGB colorspace
    ALImage* img_cam = (ALImage*)camProxy->getImageLocal(cameraId);

    // Create an openCv Mat header to convert the aldebaran AlImage image.
    // To convert the aldebaran image only the data are of it are assigned to the openCv image.
    Mat img = Mat(Size(img_cam->getWidth(), img_cam->getHeight()), CV_8UC3);
    uchar* img_data = (uchar*)malloc(img_cam->getSize());
    memcpy(img_data, (uchar*)img_cam->getData(), img_cam->getSize());
    img.data = img_data;

    // release the camara imge
    camProxy->releaseImage(cameraId);

    return img;
}

/** @brief initLeds
  *
  * Initialze the led groups.
  */
void Nao::initLeds()
{
    // Create a vector to access all RED leds
    eyeLedsRedGroup.push_back("Face/Led/Red/Left/0Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Left/45Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Left/90Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Left/135Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Left/180Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Left/225Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Left/270Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Left/315Deg/Actuator/Value");

    eyeLedsRedGroup.push_back("Face/Led/Red/Right/0Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Right/45Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Right/90Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Right/135Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Right/180Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Right/225Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Right/270Deg/Actuator/Value");
    eyeLedsRedGroup.push_back("Face/Led/Red/Right/315Deg/Actuator/Value");

    // Create a vector to access all GREEN leds
    eyeLedsGreenGroup.push_back("Face/Led/Green/Left/0Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Left/45Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Left/90Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Left/135Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Left/180Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Left/225Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Left/270Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Left/315Deg/Actuator/Value");

    eyeLedsGreenGroup.push_back("Face/Led/Green/Right/0Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Right/45Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Right/90Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Right/135Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Right/180Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Right/225Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Right/270Deg/Actuator/Value");
    eyeLedsGreenGroup.push_back("Face/Led/Green/Right/315Deg/Actuator/Value");

    //Create a vector to access all BLUE leds
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Left/0Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Left/45Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Left/90Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Left/135Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Left/180Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Left/225Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Left/270Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Left/315Deg/Actuator/Value");

    eyeLedsBlueGroup.push_back("Face/Led/Blue/Right/0Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Right/45Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Right/90Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Right/135Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Right/180Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Right/225Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Right/270Deg/Actuator/Value");
    eyeLedsBlueGroup.push_back("Face/Led/Blue/Right/315Deg/Actuator/Value");

    // create the led groups
    ledProxy->createGroup(EYE_LEDS_GREEN, eyeLedsGreenGroup);
    ledProxy->createGroup(EYE_LEDS_RED, eyeLedsRedGroup);
    ledProxy->createGroup(EYE_LEDS_BLUE, eyeLedsBlueGroup);

    // set the intensity of all led groups to maximum
    ledProxy->setIntensity(EYE_LEDS_BLUE, 1);
    ledProxy->setIntensity(EYE_LEDS_RED, 1);
    ledProxy->setIntensity(EYE_LEDS_GREEN, 1);

    // initiall shwich off all leds
    ledProxy->off(EYE_LEDS_RED);
    ledProxy->off(EYE_LEDS_BLUE);
    ledProxy->off(EYE_LEDS_GREEN);

    // set the led status
    eyeLedsRedOn = false;
    eyeLedsGreenOn = false;
    eyeLedsBlueOn = false;
}

/** @brief initProxies
  *
  * Function to set up all neccessary Proxies.
  */
void Nao::initProxies()
{
    // initialze all neccessary proxies
    //camProxy = new ALVideoDeviceProxy(parentBroker);
    behaviourProxy = new ALBehaviorManagerProxy(parentBroker);
    ledProxy = new ALLedsProxy(parentBroker);
    //memProxy = new ALMemoryProxy(parentBroker);
}

/** @brief trackPointWithHead
  *
  * Lets NAOs head move to the given x, y coordinate in camera image.
  */
void Nao::trackPointWithHead(int x, int y)
{
    // TODO
}

/** @brief switchEyeLedsBlueOn
  *
  * @todo: document this function
  */
void Nao::switchEyeLedsBlueOn()
{
    if(!eyeLedsBlueOn)
    {
        ledProxy->on(EYE_LEDS_BLUE);
        eyeLedsBlueOn = true;
    }
}

/** @brief switchEyeLedsBlueOff
  *
  * @todo: document this function
  */
void Nao::switchEyeLedsBlueOff()
{
    if(eyeLedsBlueOn)
    {
        ledProxy->off(EYE_LEDS_BLUE);
        eyeLedsBlueOn = false;
    }
}

/** @brief switchEyeLedsRedOn
  *
  * @todo: document this function
  */
void Nao::switchEyeLedsRedOn()
{
    if(!eyeLedsRedOn)
    {
        ledProxy->on(EYE_LEDS_RED);
        eyeLedsRedOn = true;
    }
}

/** @brief switchEyeLedsRedOff
  *
  * @todo: document this function
  */
void Nao::switchEyeLedsRedOff()
{
    if(eyeLedsRedOn)
    {
        ledProxy->off(EYE_LEDS_RED);
        eyeLedsRedOn = false;
    }
}

/** @brief switchEyeLedsGreenOn
  *
  * @todo: document this function
  */
void Nao::switchEyeLedsGreenOn()
{
    if(!eyeLedsGreenOn)
    {
        ledProxy->on(EYE_LEDS_GREEN);
        eyeLedsGreenOn = true;
    }
}

/** @brief switchEyeLedsGreenOff
  *
  * @todo: document this function
  */
void Nao::switchEyeLedsGreenOff()
{
    if(eyeLedsGreenOn)
    {
        ledProxy->off(EYE_LEDS_GREEN);
        eyeLedsGreenOn = false;
    }
}

/** @brief moveArmRightUp
  *
  * @todo: document this function
  */
void Nao::moveArmRightUp() {
    doBehaviour(ARM_RIGHT_UP);
}

/** @brief moveArmLeftUp
  *
  * @todo: document this function
  */
void Nao::moveArmLeftUp() {
    doBehaviour(ARM_LEFT_UP);
}

/** @brief moveArmBothUp
  *
  * @todo: document this function
  */
void Nao::moveArmBothUp() {
    doBehaviour(ARM_BOTH_UP);
}

/** @brief standUp
  *
  * @todo: document this function
  */
void Nao::standUp() {
    doBehaviour(STAND);
}

/** @brief updateStatus
  *
  * @todo: document this function
  */
void Nao::updateStatus(Gesture gesture)
{
    fsmStatus = fsmMask[gesture][fsmStatus];
    switch(fsmStatus)
    {
    case LEFT_UP_RIGHT_DOWN:
        cout<<"        RIGHT_UP"<<endl;
        doBehaviour(ARM_RIGHT_UP);
        break;
    case LEFT_DOWN_RIGHT_UP:
        cout<<"        LEFT_UP"<<endl;
        doBehaviour(ARM_LEFT_UP);
        break;
    case BOTH_UP:
        cout<<"        BOTH_UP"<<endl;
        doBehaviour(ARM_BOTH_UP);
        break;
    case BOTH_DOWN:
        cout<<"        BOTH DOWN"<<endl;
        doBehaviour(STAND);
        break;
    default:
        break;
    }
}

/** @brief initFsmMask
  *
  * @todo: document this function
  */
void Nao::initFsm()
{
    //fsmMaks[Gesture][FsmStatus] = FsmStatus;
    fsmMask[LEFT_FLIP_DOWN][LEFT_UP_RIGHT_DOWN]  = BOTH_DOWN;
    fsmMask[LEFT_FLIP_DOWN][LEFT_DOWN_RIGHT_UP]  = LEFT_DOWN_RIGHT_UP;
    fsmMask[LEFT_FLIP_DOWN][BOTH_DOWN]           = BOTH_DOWN;
    fsmMask[LEFT_FLIP_DOWN][BOTH_UP]             = LEFT_DOWN_RIGHT_UP;

    fsmMask[LEFT_FLIP_UP][LEFT_UP_RIGHT_DOWN]    = LEFT_UP_RIGHT_DOWN;
    fsmMask[LEFT_FLIP_UP][LEFT_DOWN_RIGHT_UP]    = BOTH_UP;
    fsmMask[LEFT_FLIP_UP][BOTH_DOWN]             = LEFT_UP_RIGHT_DOWN;
    fsmMask[LEFT_FLIP_UP][BOTH_UP]               = BOTH_UP;

    fsmMask[RIGHT_FLIP_DOWN][LEFT_UP_RIGHT_DOWN] = LEFT_UP_RIGHT_DOWN;
    fsmMask[RIGHT_FLIP_DOWN][LEFT_DOWN_RIGHT_UP] = BOTH_DOWN;
    fsmMask[RIGHT_FLIP_DOWN][BOTH_DOWN]          = BOTH_DOWN;
    fsmMask[RIGHT_FLIP_DOWN][BOTH_UP]            = LEFT_UP_RIGHT_DOWN;

    fsmMask[RIGHT_FLIP_UP][LEFT_UP_RIGHT_DOWN]   = BOTH_UP;
    fsmMask[RIGHT_FLIP_UP][LEFT_DOWN_RIGHT_UP]   = LEFT_DOWN_RIGHT_UP;
    fsmMask[RIGHT_FLIP_UP][BOTH_DOWN]            = LEFT_DOWN_RIGHT_UP;
    fsmMask[RIGHT_FLIP_UP][BOTH_UP]              = BOTH_UP;

    fsmStatus = BOTH_DOWN;
}

/** @brief ~Nao
  *
  * @todo: document this function
  */
Nao::~Nao()
{
    // unsubscribe from naos camera
    camProxy->unsubscribe(cameraId);

    free(behaviourProxy);
    free(ledProxy);
    free(camProxy);
    free(memProxy);
}

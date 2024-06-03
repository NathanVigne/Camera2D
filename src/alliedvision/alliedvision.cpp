#include "alliedvision.h"

/*! \fn AlliedVision::~AlliedVision()

    Destructor of the AlliedVision class.
    We close all opened ressources

*/
AlliedVision::~AlliedVision()
{
    std::clog << "AlliedVision :: destructor" << std::endl;
}

/*! \fn void AlliedVision::Start()

    AlliedVision implementation of the public Interface function to
    start continuous aquisition. If camera already running does nothing
    Error are logged;

*/
void AlliedVision::Start()
{

}


/*! \fn void AlliedVision::Stop()

    AlliedVision implementation of the public Interface function to
    stop aquisition. If camera already running it first
    stop aquisition then start a single shot event !

*/
void AlliedVision::Stop()
{

}

/*! \fn void AlliedVision::SingleShot()

    Thorlabs implementation of the public Interface function to
    start single shot aquisition. If camera already running it first
    stop aquisition then start a single shot event !

*/
void AlliedVision::SingleShot()
{

}

/*! \fn void AlliedVision::SetExposure(long long exposure_us)

    Thorlabs implementation of the public Interface function to
    set the camera exposure. the exposure must be in µs
    Error are logged;

*/
void AlliedVision::SetExposure(const long long exposure_us)
{

}

/*! \fn long long AlliedVision::GetExposure()

    AlliedVision implementation of the public Interface function to
    get the camera exposure. the exposure must is in µs
    Error are logged;

*/
long long AlliedVision::GetExposure()
{
    return 0;
}

/*! \fn void AlliedVision::SetGain(const double gain_db)

    AlliedVision implementation of the public Interface function to
    set the camera gain. the gain must be in dB
    Error are logged;

*/
void AlliedVision::SetGain(const double gain_db)
{

}

/*! \fn int AlliedVision::Connect(std::string ID)

    AlliedVision implementation of the connect function.
    From a string ID we connect to the AlliedVision camera
    and setting the camera handle.
    Errors are logged and the function return 0 is succesfull
    or -1 ortherwise.

*/
int AlliedVision::Connect(std::string ID)
{
    return -1;
}


/*! \fn int AlliedVision::Disconnect(std::string ID)

    AlliedVision implementation of the disconnect function.
    This function disconnect the openned camera.
    Errors are logged.

*/
void AlliedVision::Disconnect()
{

}

/*! \fn CamNamesIDs AlliedVision::SearchCam()

    AlliedVision implementation of the SearchCam function.
    This function open ressources if not already done. then look
    for connected cameras. If camera are found it returns the name/ID
    for each camera. Otherwise it return an empty struct.
    Errors are logged.

*/
CamNamesIDs AlliedVision::SearchCam()
{
    CamNamesIDs dummy;
    std::vector<std::string> names;
    names.push_back("AL_TestName");
    std::vector<std::string> ids;
    ids.push_back("0");
    dummy.names = names;
    dummy.ids = ids;

    return dummy;
}


/*! \fn void AlliedVision::Initialize()

    AlliedVision implementation of the camera initialization.
    This function must prepare the camera for aquisition
    by setting the callback function, getting relevant information such as
    min/max gain, min/max exposure, ...
    Errors are logged.

*/
void AlliedVision::Initialize()
{

}


/*!
 * \brief AlliedVision::setFrameReadyCallback
 * \param frameReadyCallback
 *
 * Setting the callback for when frame is ready.
 * the callback function must be set in mainWindow or mainDisplay
 *
 */
void AlliedVision::setFrameReadyCallback(std::function<void ()> frameReadyCallback)
{

}


/*!
 * \brief AlliedVision::setDisconnectCbck
 * \param disconnectCbck
 *
 * Setting the callback for when cam is disconnect.
 * the callback function must be set in mainWindow or mainDisplay
 *
 */
void AlliedVision::setDisconnectCbck(std::function<void ()> disconnectCbck)
{

}

/*!
 * \brief AlliedVision::setConnectCbck
 * \param connectCbck
 *
 * Setting the callback for when cam is connected.
 * the callback function must be set in mainWindow or mainDisplay
 *
 */
void AlliedVision::setConnectCbck(std::function<void ()> connectCbck)
{

}

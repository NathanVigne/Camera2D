// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the MTUSBDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// MTUSBDLL_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
typedef int SDK_RETURN_CODE;
typedef unsigned int DEV_HANDLE;

#ifdef SDK_EXPORTS
#define SDK_API extern "C" __declspec(dllexport) SDK_RETURN_CODE _cdecl
#define SDK_HANDLE_API extern "C" __declspec(dllexport) DEV_HANDLE _cdecl
#else
#define SDK_API extern "C" __declspec(dllimport) SDK_RETURN_CODE _cdecl
#define SDK_HANDLE_API extern "C" __declspec(dllimport) DEV_HANDLE _cdecl
#endif

#define RAWDATA_IMAGE   0
#define BMPDATA_IMAGE   1

#pragma pack(1)

typedef struct {
    int XStart;
    int YStart;
    int GreenGain;
    int BlueGain;
    int RedGain;
    int ExposureTime;
    // Start from SDK V1.4.0.0, we have 3 more fields:
    int TriggerOccurred;
    int Reserved1;
    int Reserved2;
} TImageAttachData;

typedef struct {
    int Revision;
    // For Image Capture
    int Resolution;
    int BinMode;
    int XStart;
    int YStart;
    int GreenGain;
    int BlueGain;
    int RedGain;
    int MaxExposureTimeIndex;
    int ExposureTime;
    // For Image Rendor
    bool ImageRendorFitWindow;
    int Gamma;
    int Contrast;
    int Bright;
    int SharpLevel;
    bool BWMode;
    bool HorizontalMirror;
    bool VerticalFlip;
    // For Capture Files.
    int CatchFrames;
    bool IsAverageFrame;
    bool IsCatchRAW;
    bool IsRawGraph;
    bool IsCatchJPEG;
    bool CatchIgnoreSkip;
} TImageControl;
#pragma pack()

typedef TImageControl *PImageCtl;
typedef void (* CallBackFunc)( int ImageSequenceNo, char *FileName );
typedef void (* GetFrameCallBack)(int FrameType, int Row, int Col, 
                                  TImageAttachData* Attributes, unsigned char *BytePtr );
typedef void (* DeviceCallBack)(int ErrorType );                                  

// Export functions:
SDK_API MTUSB_InitDevice( void );
SDK_API MTUSB_UnInitDevice( void );
SDK_HANDLE_API MTUSB_OpenDevice( int deviceID );
SDK_HANDLE_API MTUSB_ShowOpenDeviceDialog( void );
SDK_API MTUSB_GetModuleNo( DEV_HANDLE DevHandle, char *ModuleNo );
SDK_API MTUSB_GetSerialNo( DEV_HANDLE DevHandle, char *SerialNo );
SDK_API MTUSB_StartCameraEngine( HWND ParentHandle, DEV_HANDLE DevHandle );
SDK_API MTUSB_StopCameraEngine( DEV_HANDLE DevHandle );
SDK_API MTUSB_SetCameraWorkMode( DEV_HANDLE DevHandle, int WorkMode );
SDK_API MTUSB_SetExternalParameters( DEV_HANDLE DevHandle, bool AutoLoop, bool IsRawGraph,
                                     bool IsJPEG, char *FilePath, char *FileName);
SDK_API MTUSB_WaitingExternalTrigger( DEV_HANDLE DevHandle, bool StartWait, CallBackFunc Aproc );
SDK_API MTUSB_ShowFrameControlPanel( DEV_HANDLE DevHandle, bool IsTriggerModeAllow, bool CloseParent, 
                     char *Title, int Left, int Top);
SDK_API MTUSB_HideFrameControlPanel( DEV_HANDLE DevHandle );
SDK_API MTUSB_ShowVideoWindow( DEV_HANDLE DevHandle, int Top, int Left,
                   int Width, int Height );
SDK_API MTUSB_DisplayVideoWindow( int IsDisplayVideoWindow );
SDK_API MTUSB_StartFrameGrab( DEV_HANDLE DevHandle, int TotalFrames );
SDK_API MTUSB_StopFrameGrab( DEV_HANDLE DevHandle );
SDK_API MTUSB_GetFrameSetting( DEV_HANDLE DevHandle, PImageCtl SettingPtr);
SDK_API MTUSB_SetFrameSetting( DEV_HANDLE DevHandle, PImageCtl SettingPtr);
SDK_API MTUSB_SetResolution( DEV_HANDLE DevHandle, PImageCtl SettingPtr);
SDK_API MTUSB_SetStartPosition( DEV_HANDLE DevHandle, PImageCtl SettingPtr);
SDK_API MTUSB_SetGain( DEV_HANDLE DevHandle, PImageCtl SettingPtr);
SDK_API MTUSB_SetExposureTime( DEV_HANDLE DevHandle, PImageCtl SettingPtr);
SDK_API MTUSB_SetGammaValue( DEV_HANDLE DevHandle, PImageCtl SettingPtr);
SDK_API MTUSB_SetGammaTable( DEV_HANDLE DevHandle, unsigned char *GammaTable );
SDK_API MTUSB_SetShowMode( DEV_HANDLE DevHandle, PImageCtl SettingPtr);
SDK_API MTUSB_SetWhiteBalance( DEV_HANDLE DevHandle );
SDK_API MTUSB_SetFrameRateLevel( DEV_HANDLE DevHanlde, int RateLevel );
SDK_API MTUSB_SetAutoExposure( DEV_HANDLE DevHanlde, bool AutoExposureOn, bool ShowExposureMark );
SDK_API MTUSB_GetCurrentFrameRate( DEV_HANDLE DevHandle );
SDK_API MTUSB_GetLastBMPFrame( DEV_HANDLE DevHandle, char *FileName );
SDK_API MTUSB_GetCurrentFrame( DEV_HANDLE DevHandle, int FrameType, unsigned char *Buffer );
SDK_API MTUSB_GetCurrentFrame2( DEV_HANDLE DevHandle, int FrameType, unsigned char *Buffer, unsigned char *ImagePropertyBuf );
SDK_API MTUSB_InstallFrameHooker( DEV_HANDLE DevHandle, bool IsMaxFrameRate, int FrameType, GetFrameCallBack FrameHooker);
SDK_API MTUSB_InstallDeviceHooker( DeviceCallBack DeviceHooker );
SDK_API MTUSB_SaveFramesToFiles( DEV_HANDLE DevHandle, PImageCtl SettingPtr,
                                 char *FilePath, char *FileName );
SDK_API MTUSB_SetLEDBrightness( DEV_HANDLE DevHandle, unsigned char LEDChannel, 
                                unsigned char Brightness );
SDK_API MTUSB_SetGPIOConifg( DEV_HANDLE DevHandle, unsigned char ConfigByte );
SDK_API MTUSB_SetGPIOInOut( DEV_HANDLE DevHandle, unsigned char OutputByte,
                             unsigned char *InputBytePtr );


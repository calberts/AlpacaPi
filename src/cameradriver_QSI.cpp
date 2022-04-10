//**************************************************************************
//*	Name:			cameradriver_QSI.cpp
//*
//*	Author:			Mark Sproul (C) 2022
//*
//*	Description:	C++ Driver for Alpaca protocol
//*
//*****************************************************************************
//*	AlpacaPi is an open source project written in C/C++
//*
//*	Use of this source code for private or individual use is granted
//*	Use of this source code, in whole or in part for commercial purpose requires
//*	written agreement in advance.
//*
//*	You may use or modify this source code in any way you find useful, provided
//*	that you agree that the author(s) have no warranty, obligations or liability.  You
//*	must determine the suitability of this source code for your use.
//*
//*	Redistributions of this source code must retain this copyright notice.
//*****************************************************************************
//*
//*	References:
//*			https://qsimaging.com/
//*****************************************************************************
//*	Edit History
//*****************************************************************************
//*	<MLS>	=	Mark L Sproul
//*****************************************************************************
//*	Apr  7,	2022	<MLS> Created cameradriver_QSI.cpp
//*	Apr  8,	2022	<MLS> Got QSI library to compile and install
//*****************************************************************************

#if defined(_ENABLE_CAMERA_) && defined(_ENABLE_QSI_)

#include	<string>


#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"

#include	"alpaca_defs.h"
#include	"alpacadriver.h"
#include	"alpacadriver_helper.h"
#include	"eventlogging.h"
#include	"cameradriver.h"
#include	"cameradriver_QSI.h"
#include	"linuxerrors.h"

//*	qsiapi-7.6.0/lib/
#include	"qsiapi.h"
#include	"QSIError.h"


//**************************************************************************************
void	CreateQSI_CameraObjects(void)
{
int				iii;
//std::string		serial("");
//std::string		desc("");
std::string		info	=	"";
std::string		modelNumber("");
std::string		camSerial[QSICamera::MAXCAMERAS];
std::string		camDesc[QSICamera::MAXCAMERAS];
std::string		text;
std::string		last("");
int				numCamerasFound;

QSICamera		qsiCam;

	CONSOLE_DEBUG(__FUNCTION__);

	numCamerasFound	=	0;
	qsiCam.put_UseStructuredExceptions(true);
	try
	{
		qsiCam.get_DriverInfo(info);
		CONSOLE_DEBUG(info.c_str());
	//	std::cout << "qsiapi version: " << info << "\n";

		AddLibraryVersion("camera", "QSI", info.c_str());
		LogEvent(	"camera",
					"Library version (QSI)",
					NULL,
					kASCOM_Err_Success,
					info.c_str());

		//Discover the connected cameras
		qsiCam.get_AvailableCameras(camSerial, camDesc, numCamerasFound);
		CONSOLE_DEBUG_W_NUM("numCamerasFound\t=", numCamerasFound);
	}
	catch (std::runtime_error &err)
	{
	std::string		text;
	std::string		last("");
		text	=	err.what();
//		std::cout << text << "\n";
		CONSOLE_DEBUG(text.c_str());

		qsiCam.get_LastError(last);
		CONSOLE_DEBUG(last.c_str());
		CONSOLE_ABORT(__FUNCTION__);
	}

	if (numCamerasFound > 0)
	{
		for (iii=0; iii < numCamerasFound; numCamerasFound++)
		{
			new CameraDriverQSI(iii, camSerial[iii]);
		}
	}
	else
	{
		//*	for debugging without a camera
		new CameraDriverQSI(0, camSerial[0]);
	}
}

//**************************************************************************************
CameraDriverQSI::CameraDriverQSI(	const int	deviceNum,
									std::string qsiSerialNumber)
	:CameraDriver()
{
	CONSOLE_DEBUG(__FUNCTION__);
	cCameraID				=	deviceNum;

	//*	set defaults
	strcpy(cDeviceManufAbrev,		"QSI");
	strcpy(cCommonProp.Name,		"QSI");
	strcpy(cCommonProp.Description,	"QSI");
	strcpy(cCommonProp.Name,		"QSI");


	cQSIserialNumber	=	qsiSerialNumber;
	cQSIcamera.put_UseStructuredExceptions(false);

	cQSIcamera.put_SelectCamera(cQSIserialNumber);
	cQSIcamera.put_IsMainCamera(true);


	ReadQSIcameraInfo();

	strcpy(cCommonProp.Description, cDeviceManufacturer);
	strcat(cCommonProp.Description, " - Model:");
	strcat(cCommonProp.Description, cCommonProp.Name);

	try
	{
		//*	set to the default camera, only one attached
		cQSIcamera.put_QSISelectedDevice(std::string(""));
	}
	catch (std::runtime_error &err)
	{
	std::string		text;
	std::string		last("");
		text	=	err.what();
//		std::cout << text << "\n";
		CONSOLE_DEBUG(text.c_str());

		cQSIcamera.get_LastError(last);
		CONSOLE_DEBUG(last.c_str());

		CONSOLE_ABORT(__FUNCTION__);
	}


#ifdef _USE_OPENCV_
	sprintf(cOpenCV_ImgWindowName, "%s-%d", cCommonProp.Name, cCameraID);
#endif // _USE_OPENCV_

}


//**************************************************************************************
// Destructor
//**************************************************************************************
CameraDriverQSI::~CameraDriverQSI(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
}

//*****************************************************************************
void	CameraDriverQSI::ReadQSIcameraInfo(void)
{
int			qsi_Result;
bool		canSetTemp;
bool		hasFilters;
long		xsize;
long		ysize;
std::string	serial("");
std::string	desc("");
std::string	info	=	"";
std::string	modelNumber("");
std::string	lastError("");


	CONSOLE_DEBUG(__FUNCTION__);

	strcpy(cDeviceManufacturer,	"QSI");
	cCameraProp.CameraXsize	=	4000;
	cCameraProp.CameraYsize	=	3000;
	cCameraProp.NumX		=	4000;
	cCameraProp.NumY		=	3000;

	CONSOLE_DEBUG(__FUNCTION__);
	//--------------------------------------------------------------
	//*	Get Model Number
	qsi_Result	=	cQSIcamera.get_ModelNumber(modelNumber);
	if (qsi_Result == 0)
	{
		strcpy(cCommonProp.Name,		modelNumber.c_str());
		CONSOLE_DEBUG_W_STR("QSI-modelNumber\t=",	cCommonProp.Name);
	}
	else if (qsi_Result == QSI_NOTCONNECTED)
	{
		cQSIcamera.get_LastError(lastError);
		CONSOLE_DEBUG_W_STR("QSI Result\t=",	lastError.c_str());
	}

	//--------------------------------------------------------------
	//*	Get Description
	qsi_Result	=	cQSIcamera.get_Description(desc);
	if (qsi_Result == 0)
	{
		strcpy(cCommonProp.Description,		desc.c_str());
		CONSOLE_DEBUG_W_STR("QSI-Description\t=",	cCommonProp.Description);
	}
	else
	{
		cQSIcamera.get_LastError(lastError);
		CONSOLE_DEBUG_W_STR("QSI Result\t=",	lastError.c_str());
	}

	//--------------------------------------------------------------
	//*	Get the dimensions of the CCD
	qsi_Result	=	cQSIcamera.get_CameraXSize(&xsize);
	if (qsi_Result == 0)
	{
		cCameraProp.CameraXsize	=	xsize;
	}
	else
	{
		cQSIcamera.get_LastError(lastError);
		CONSOLE_DEBUG_W_STR("QSI Result\t=",	lastError.c_str());
	}
	qsi_Result	=	cQSIcamera.get_CameraYSize(&ysize);
	if (qsi_Result == 0)
	{
		cCameraProp.CameraYsize	=	ysize;
	}
	else
	{
		cQSIcamera.get_LastError(lastError);
		CONSOLE_DEBUG_W_STR("QSI Result\t=",	lastError.c_str());
	}

//	//--------------------------------------------------------------
//	if (qsi_Result == 0)
//	{
//	}
//	else
//	{
//		cQSIcamera.get_LastError(lastError);
//		CONSOLE_DEBUG_W_STR("QSI Result\t=",	lastError.c_str());
//	}

		// Enable the beeper
		qsi_Result	=	cQSIcamera.put_SoundEnabled(true);
		// Enable the indicator LED
		qsi_Result	=	cQSIcamera.put_LEDEnabled(true);
		// Set the fan mode
		qsi_Result	=	cQSIcamera.put_FanMode(QSICamera::fanQuiet);
		// Query the current flush mode setting
		qsi_Result	=	cQSIcamera.put_PreExposureFlush(QSICamera::FlushNormal);

		// Query if the camera can control the CCD temp
		qsi_Result	=	cQSIcamera.get_CanSetCCDTemperature(&canSetTemp);
		if (canSetTemp)
		{
			// Set the CCD temp setpoint to 10.0C
			qsi_Result	=	cQSIcamera.put_SetCCDTemperature(10.0);
			// Enable the cooler
			qsi_Result	=	cQSIcamera.put_CoolerOn(true);
		}

		if (modelNumber.substr(0,1) == "6")
		{
			qsi_Result	=	cQSIcamera.put_ReadoutSpeed(QSICamera::FastReadout);
		}

		// Does the camera have a filer wheel?
		qsi_Result	=	cQSIcamera.get_HasFilterWheel(&hasFilters);
		if ( hasFilters)
		{
			// Set the filter wheel to position 1 (0 based position)
			qsi_Result	=	cQSIcamera.put_Position(0);
		}

		if (modelNumber.substr(0,3) == "520" || modelNumber.substr(0,3) == "540")
		{
			qsi_Result	=	cQSIcamera.put_CameraGain(QSICamera::CameraGainHigh);
			qsi_Result	=	cQSIcamera.put_PreExposureFlush(QSICamera::FlushNormal);
		}
		//
		//////////////////////////////////////////////////////////////
		// Set image size
		//
		qsi_Result	=	cQSIcamera.put_BinX(1);
		qsi_Result	=	cQSIcamera.put_BinY(1);
		// Set the exposure to a full frame
		qsi_Result	=	cQSIcamera.put_StartX(0);
		qsi_Result	=	cQSIcamera.put_StartY(0);
		qsi_Result	=	cQSIcamera.put_NumX(xsize);
		qsi_Result	=	cQSIcamera.put_NumY(ysize);
}

//**************************************************************************
//*	sets class variable to current temp
//**************************************************************************
TYPE_ASCOM_STATUS	CameraDriverQSI::Read_SensorTemp(void)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
//double				cameraTemp_DegC;

	CONSOLE_DEBUG(__FUNCTION__);

	cLastCameraErrMsg[0]	=	0;
	if (cTempReadSupported)
	{
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_NotSupported;
		strcpy(cLastCameraErrMsg, "Temperature not supported on this camera");
	}
	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriverQSI::Write_Gain(const int newGainValue)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_InvalidOperation;

	CONSOLE_DEBUG_W_NUM(__FUNCTION__, newGainValue);

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriverQSI::Read_Gain(int *cameraGainValue)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_PropertyNotImplemented;
//double				rawGainValue;

	CONSOLE_DEBUG(__FUNCTION__);

	return(alpacaErrCode);
}

//*****************************************************************************
TYPE_ASCOM_STATUS		CameraDriverQSI::Start_CameraExposure(int32_t exposureMicrosecs)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;
double				exposureMilliSecs;
int					qsi_Result;
std::string			lastError("");

	CONSOLE_DEBUG(__FUNCTION__);

	exposureMilliSecs	=	(exposureMicrosecs * 1.0) / 1000.0;
	qsi_Result			=	cQSIcamera.StartExposure(exposureMilliSecs, true);
	if (qsi_Result == 0)
	{
		alpacaErrCode	=	kASCOM_Err_Success;
	}
	else
	{
		cQSIcamera.get_LastError(lastError);
		CONSOLE_DEBUG_W_STR("QSI Result\t=",	lastError.c_str());
	}
	return(alpacaErrCode);
}


//*****************************************************************************
bool	CameraDriverQSI::GetImage_ROI_info(void)
{
	cROIinfo.currentROIwidth		=	cCameraProp.CameraXsize;
	cROIinfo.currentROIheight		=	cCameraProp.CameraYsize;
	cROIinfo.currentROIbin			=	1;
	return(true);
}

//*****************************************************************************
TYPE_EXPOSURE_STATUS	CameraDriverQSI::Check_Exposure(bool verboseFlag)
{
//uint32_t				precentRemaining;
TYPE_EXPOSURE_STATUS	myExposureStatus;

	CONSOLE_DEBUG(__FUNCTION__);

	myExposureStatus	=	kExposure_Failed;

	return(myExposureStatus);
}

//*****************************************************************************
TYPE_ASCOM_STATUS	CameraDriverQSI::SetImageType(TYPE_IMAGE_TYPE newImageType)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;

	CONSOLE_DEBUG(__FUNCTION__);
	CONSOLE_DEBUG_W_NUM("newImageType\t=",			newImageType);

	if (1)
	{
		cROIinfo.currentROIwidth	=	cCameraProp.CameraXsize;
		cROIinfo.currentROIheight	=	cCameraProp.CameraYsize;
		cROIinfo.currentROIbin		=   1;

		switch(newImageType)
		{
			case kImageType_RAW8:
			case kImageType_RAW16:
				cROIinfo.currentROIimageType	=	kImageType_RAW8;
//+				cROIinfo.currentROIimageType	=	kImageType_RAW16;
				break;


			case kImageType_RGB24:
				cROIinfo.currentROIimageType	=	kImageType_RGB24;
				break;

			case kImageType_Y8:
				cROIinfo.currentROIimageType	=	kImageType_Y8;
				break;

			case kImageType_Invalid:
			case kImageType_last:
				break;

		}


	}
	else
	{
		CONSOLE_DEBUG("Not connected");
		alpacaErrCode	=	kASCOM_Err_NotConnected;
	}

	return(alpacaErrCode);
}


//**************************************************************************
TYPE_ASCOM_STATUS	CameraDriverQSI::Read_ImageData(void)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_NotImplemented;

	CONSOLE_DEBUG(__FUNCTION__);


	return(alpacaErrCode);
}


#endif // defined(_ENABLE_CAMERA_) && defined(_ENABLE_QSI_)
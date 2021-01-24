//*****************************************************************************
//*		controller_telescope.cpp		(c) 2021 by Mark Sproul
//*
//*
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
//*	Edit History
//*****************************************************************************
//*	Jan 22,	2021	<MLS> Created controller_telescope.cpp
//*	Jan 23,	2021	<MLS> Added Update_TelescopeRtAscension()
//*	Jan 23,	2021	<MLS> Added Update_TelescopeDeclination()
//*	Jan 24,	2021	<MLS> Converted TelescopeControler to use properties struct
//*****************************************************************************


#ifdef _ENABLE_CTRL_TELESCOPE_


#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>


#include "opencv/highgui.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc/imgproc_c.h"

#include	"discovery_lib.h"


#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"


#define	kWindowWidth	700
#define	kWindowHeight	550

#include	"windowtab_telescope.h"
#include	"windowtab_about.h"

#include	"controller.h"
#include	"controller_telescope.h"


//**************************************************************************************
enum
{
	kTab_Telescope	=	1,
	kTab_About,

	kTab_Count

};

//**************************************************************************************
ControllerTelescope::ControllerTelescope(	const char			*argWindowName,
											struct sockaddr_in	*deviceAddress,
											const int			port,
											const int			deviceNum)
	:Controller(argWindowName, kWindowWidth,  kWindowHeight)
{

	cAlpacaDevNum			=	deviceNum;
	cFirstDataRead			=	true;
	cLastUpdate_milliSecs	=	millis();

	if (deviceAddress != NULL)
	{
		cDeviceAddress	=	*deviceAddress;
		cPort			=	port;
		cValidIPaddr	=	true;
	}
	cLastUpdate_milliSecs	=	0;
	SetupWindowControls();

	//*	zero out all of the Telescope ASCOM properties
	memset(&cTelescopeProp, 0, sizeof(TYPE_TelescopeProperties));


	SetWidgetText(kTab_About,		kAboutBox_AlpacaDrvrVersion,		gFullVersionString);
}




//**************************************************************************************
// Destructor
//**************************************************************************************
ControllerTelescope::~ControllerTelescope(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
	if (cTelescopeTabObjPtr != NULL)
	{
		CONSOLE_DEBUG("Deleting cTelescopeTabObjPtr");
		delete cTelescopeTabObjPtr;
		cTelescopeTabObjPtr	=	NULL;
	}
	if (cAboutBoxTabObjPtr != NULL)
	{
		CONSOLE_DEBUG("Deleting cAboutBoxTabObjPtr");
		delete cAboutBoxTabObjPtr;
		cAboutBoxTabObjPtr	=	NULL;
	}
}


//**************************************************************************************
void	ControllerTelescope::SetupWindowControls(void)
{

//	CONSOLE_DEBUG(__FUNCTION__);

	SetTabCount(kTab_Count);
	SetTabText(kTab_Telescope,	"Telescope");
	SetTabText(kTab_About,		"About");


	cTelescopeTabObjPtr	=	new WindowTabTelescope(cWidth, cHeight, cBackGrndColor, cWindowName);
	if (cTelescopeTabObjPtr != NULL)
	{
		SetTabWindow(kTab_Telescope,	cTelescopeTabObjPtr);
		cTelescopeTabObjPtr->SetParentObjectPtr(this);
	}

	cAboutBoxTabObjPtr		=	new WindowTabAbout(	cWidth, cHeight, cBackGrndColor, cWindowName);
	if (cAboutBoxTabObjPtr != NULL)
	{
		SetTabWindow(kTab_About,	cAboutBoxTabObjPtr);
		cAboutBoxTabObjPtr->SetParentObjectPtr(this);
	}

	SetWidgetFont(kTab_Telescope,	kTelescope_IPaddr, kFont_Medium);

	//*	display the IPaddres/port
	if (cValidIPaddr)
	{
	char	lineBuff[128];
	char	ipString[32];

		inet_ntop(AF_INET, &cDeviceAddress.sin_addr.s_addr, ipString, INET_ADDRSTRLEN);

//		PrintIPaddressToString(cDeviceAddress.sin_addr.s_addr, ipString);
		sprintf(lineBuff, "%s:%d/%d", ipString, cPort, cAlpacaDevNum);
		SetWindowIPaddrInfo(lineBuff, true);
	}
}

//**************************************************************************************
void	ControllerTelescope::RunBackgroundTasks(void)
{
uint32_t	currentMilliSecs;
uint32_t	deltaMilliSecs;
bool		validData;

//	CONSOLE_DEBUG_W_STR(__FUNCTION__, cWindowName);
	if (cReadStartup)
	{
		AlpacaGetStartupData();
		cReadStartup	=	false;
	}

	currentMilliSecs	=	millis();
	deltaMilliSecs		=	currentMilliSecs - cLastUpdate_milliSecs;
	if (deltaMilliSecs > 2000)
	{
		validData	=	AlpacaGetStatus();
		cLastUpdate_milliSecs	=	millis();
	}
}

//*****************************************************************************
//*	this routine gets called one time to get the info on the telescope that does not change
//*****************************************************************************
bool	ControllerTelescope::AlpacaGetStartupData(void)
{
bool	validData;
char	returnString[256];

	CONSOLE_DEBUG_W_STR(__FUNCTION__, cWindowName);

	//===============================================================
	//*	get supportedactions
	validData	=	AlpacaGetSupportedActions("telescope", cAlpacaDevNum);
	if (validData)
	{
		CONSOLE_DEBUG_W_STR("Valid supported actions:", cWindowName);
//		UpdateSupportedActions();
	}
	else
	{
		CONSOLE_DEBUG("Read failure - supportedactions");
		cReadFailureCnt++;
	}
	if (cHas_readall)
	{
		validData	=	AlpacaGetStatus_ReadAll("telescope", cAlpacaDevNum);
	}
	else
	{
		validData	=	AlpacaGetStartupData_TelescopeOneAAT();

		//========================================================
		validData	=	AlpacaGetStringValue(	"telescope", "description",	NULL,	returnString);
		if (validData)
		{
			SetWidgetText(kTab_Telescope, kTelescope_AlpacaDrvrVersion, returnString);
		}
	}


	SetWidgetValid(kTab_Telescope,	kTelescope_Readall,		cHas_readall);
	SetWidgetValid(kTab_About,		kAboutBox_Readall,		cHas_readall);

	return(validData);
}

//*****************************************************************************
void	ControllerTelescope::AlpacaDisplayErrorMessage(const char *errorMsgString)
{
	CONSOLE_DEBUG_W_STR("Alpaca error=", errorMsgString);
	SetWidgetText(kTab_Telescope, kTelescope_ErrorMsg, errorMsgString);
}


//*****************************************************************************
void	ControllerTelescope::AlpacaProcessReadAll(	const char	*deviceType,
													const int	deviceNum,
													const char	*keywordString,
													const char *valueString)
{
//	CONSOLE_DEBUG_W_2STR("json=",	keywordString, valueString);
	if (strcasecmp(deviceType, "Telescope") == 0)
	{
		AlpacaProcessReadAll_Telescope(deviceNum, keywordString, valueString);
	}
#ifdef _ENABLE_SKYTRAVEL_
	else if (strcasecmp(valueString, "Telescope") == 0)
	{
		//*	you get the idea
	}
#endif // _ENABLE_SKYTRAVEL_
}


//*****************************************************************************
void	ControllerTelescope::AlpacaProcessSupportedActions(	const char	*deviceType,
															const int	deviveNum,
															const char	*valueString)
{
	CONSOLE_DEBUG_W_STR(__FUNCTION__, valueString);

	AlpacaProcessSupportedActions_Telescope(deviveNum, valueString);
}


//*****************************************************************************
bool	ControllerTelescope::AlpacaGetStatus(void)
{
bool	validData;
bool	previousOnLineState;

//	CONSOLE_DEBUG(__FUNCTION__);

	previousOnLineState	=	cOnLine;
	if (cHas_readall)
	{
		validData	=	AlpacaGetStatus_ReadAll("telescope", cAlpacaDevNum);
	}
	else
	{
		validData	=	AlpacaGetStatus_TelescopeOneAAT();
	}

	if (validData)
	{
		if (cOnLine == false)
		{
			//*	if we were previously off line, force reading startup again
			cReadStartup	=	true;
		}
		cOnLine	=	true;

		//*	update telescope tracking
		if (cTelescopeProp.Tracking)
		{
			SetWidgetText(kTab_Telescope,		kTelescope_TrackingStatus, "Tracking On");
			SetWidgetBGColor(kTab_Telescope,	kTelescope_TrackingStatus,	CV_RGB(0,	255,	0));
			SetWidgetTextColor(kTab_Telescope,	kTelescope_TrackingStatus,	CV_RGB(0,	0,	0));
		}
		else
		{
			SetWidgetText(kTab_Telescope,		kTelescope_TrackingStatus, "Tracking Off");
			SetWidgetBGColor(kTab_Telescope,	kTelescope_TrackingStatus,	CV_RGB(0,	0,	0));
			SetWidgetTextColor(kTab_Telescope,	kTelescope_TrackingStatus,	CV_RGB(255,	0,	0));
		}

		//*	update slewing
		if (cTelescopeProp.Slewing)
		{
			SetWidgetText(kTab_Telescope,		kTelescope_SlewingStatus,	"Slewing On");
			SetWidgetBGColor(kTab_Telescope,	kTelescope_SlewingStatus,	CV_RGB(0,	255,	0));
			SetWidgetTextColor(kTab_Telescope,	kTelescope_SlewingStatus,	CV_RGB(0,	0,	0));
		}
		else
		{
			SetWidgetText(kTab_Telescope,		kTelescope_SlewingStatus, "Slewing Off");
			SetWidgetBGColor(kTab_Telescope,	kTelescope_SlewingStatus,	CV_RGB(0,	0,	0));
			SetWidgetTextColor(kTab_Telescope,	kTelescope_SlewingStatus,	CV_RGB(255,	0,	0));
		}


	}
	else
	{
		cOnLine	=	false;
	}
	if (cOnLine != previousOnLineState)
	{
		SetWindowIPaddrInfo(NULL, cOnLine);
	}

	cLastUpdate_milliSecs	=	millis();
	cFirstDataRead			=	false;
	return(validData);
}

//**************************************************************************************
void	ControllerTelescope::Update_TelescopeRtAscension(void)
{
char	hhmmssString[64];

	FormatHHMMSS(cTelescopeProp.RightAscension, hhmmssString, false);
	SetWidgetText(kTab_Telescope,	kTelescope_RA_value,		hhmmssString);
}

//**************************************************************************************
void	ControllerTelescope::Update_TelescopeDeclination(void)
{
char	hhmmssString[64];

	FormatHHMMSS(cTelescopeProp.Declination, hhmmssString, true);
	SetWidgetText(kTab_Telescope,	kTelescope_DEC_value,		hhmmssString);
}


#define	_PARENT_IS_TELESCOPE_
#define	PARENT_CLASS	ControllerTelescope
#include	"controller_tscope_common.cpp"

#endif // _ENABLE_CTRL_TELESCOPE_

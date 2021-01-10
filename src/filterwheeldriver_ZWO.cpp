//**************************************************************************
//*	Name:			filterwheeldriver_ZWO.cpp
//*
//*	Author:			Mark Sproul (C) 2019
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
//*	Usage notes:	Remember to install the rules and reboot,
//*					see EFW_linux_mac_SDK/lib/README.txt
//*
//*****************************************************************************
//*	Edit History
//*****************************************************************************
//*	<MLS>	=	Mark L Sproul
//*****************************************************************************
//*	Nov 10,	2019	<MLS> Created filterwheeldriver_ZWO.cpp
//*	Nov 10,	2019	<MLS> Started on C++ version of ZWO filter wheel
//*	Nov 10,	2019	<MLS> C++ version of ZWQ EFW filterwheel working
//*	Dec 15,	2019	<MLS> Added CheckZWOrulesFile()
//*	Apr  1,	2020	<MLS> Updated to zero based indexing as per ASCOM
//*	Apr 19,	2020	<MLS> ZWO Filterwheel keeps hanging in Read_CurrentFilterPositon
//*	Apr 19,	2020	<MLS> Fixed close bug in ReadZWOfilterWheelInfo()
//*	May  4,	2020	<MLS> Add EFW version number to log event
//*****************************************************************************

#ifdef _ENABLE_FILTERWHEEL_

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<stdbool.h>
#include	<ctype.h>
#include	<stdint.h>

#include	"include/EFW_filter.h"
#include	"eventlogging.h"

#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"

#define	kEFW_rulesFile	"efw.rules"

#include	"alpacadriver.h"
#include	"alpacadriver_helper.h"
#include	"filterwheeldriver.h"
#include	"filterwheeldriver_ZWO.h"

static	void	GetEFW_ErrorMsgString(EFW_ERROR_CODE errorCode, char *errorMsg);


//**************************************************************************************
//*	this will get moved to the individual implentations later
void	CreateZWOFilterWheelObjects(void)
{
int			filterwheelCnt;
int			devNum;
bool		rulesFileOK;
char		*efwVersionString;


	CONSOLE_DEBUG(__FUNCTION__);

	efwVersionString	=	EFWGetSDKVersion();
	if (efwVersionString != NULL)
	{
		CONSOLE_DEBUG_W_STR("EFW Version=", efwVersionString);
		LogEvent(	"filterwheel",
					"Library version (ZWO-EFW)",
					NULL,
					kASCOM_Err_Success,
					efwVersionString);
		AddLibraryVersion("filterwheel", "ZWO", efwVersionString);
	}
	//*	check to make sure the rules file is present
	rulesFileOK	=	Check_udev_rulesFile(kEFW_rulesFile);
	if (rulesFileOK == false)
	{
		LogEvent(	"filterwheel",
					"Problem with ZWO-EFW rules",
					NULL,
					kASCOM_Err_Success,
					kEFW_rulesFile);
	}

	filterwheelCnt	=	EFWGetNum();
	CONSOLE_DEBUG_W_NUM("filterwheelCnt\t=", filterwheelCnt);

	for (devNum=0; devNum<filterwheelCnt; devNum++)
	{
		new FilterwheelZWO(devNum);
	}
}


//**************************************************************************************
FilterwheelZWO::FilterwheelZWO(const int argDevNum)
	:FilterwheelDriver(argDevNum)
{
bool	rulesFileOK;


	CONSOLE_DEBUG(__FUNCTION__);
	cFilterWheelDevNum	=	argDevNum;
	cNumberOfPostions	=	0;

	strcpy(cDeviceDescription, "ZWO EFW filterwheel");
	strcpy(cDriverversionStr,	EFWGetSDKVersion());

	rulesFileOK	=	Check_udev_rulesFile(kEFW_rulesFile);
	if (rulesFileOK == false)
	{
		CONSOLE_DEBUG("ZWO EFW Rules file does not appear to be installed properly");
	}
	ReadZWOfilterWheelInfo();

	//*	on startup we need to know the current position
	Read_CurrentFilterPositon(NULL);	//*	we dont care about a return value
}

//**************************************************************************************
// Destructor
//**************************************************************************************
FilterwheelZWO::~FilterwheelZWO(void)
{
EFW_ERROR_CODE	efwErrorCode;

	CONSOLE_DEBUG(__FUNCTION__);
	efwErrorCode	=	EFWClose(cFilterWheelDevNum);
	if (efwErrorCode == EFW_SUCCESS)
	{
		CONSOLE_DEBUG("EFW filter wheel closed OK");
	}
	else
	{
		CONSOLE_DEBUG("EFW filter wheel closed failed");
	}

}

//**************************************************************************************
void	FilterwheelZWO::ReadZWOfilterWheelInfo(void)
{
EFW_ERROR_CODE	efwErrorCode;
char			lineBuff[64];
bool			rulesFileOK;


	CONSOLE_DEBUG(__FUNCTION__);
	strcpy(cDeviceName, "ZWO Filterwheel");	//*	put somethere in case of failure to open
	efwErrorCode	=	EFWOpen(cFilterWheelDevNum);
	if (efwErrorCode == EFW_SUCCESS)
	{
		cSuccesfullOpens++;
		cFilterWheelConnected	=	true;
		efwErrorCode	=	EFWGetProperty(cFilterWheelDevNum, &cFilterwheelInfo);
		if (efwErrorCode == EFW_SUCCESS)
		{
			LogEvent(	"filterwheel",
						"ZWO EFW connected",
						NULL,
						kASCOM_Err_Success,
						cFilterwheelInfo.Name);
		//	strcpy(cDeviceName, cFilterwheelInfo.Name);
			cNumberOfPostions	=	cFilterwheelInfo.slotNum;

			sprintf(cDeviceName, "ZWO %s-%d", cFilterwheelInfo.Name, cNumberOfPostions);
		}
		else
		{
			CONSOLE_DEBUG_W_NUM("EFWGetProperty->efwErrorCode\t=", efwErrorCode);
			GetEFW_ErrorMsgString(efwErrorCode, lineBuff);
			LogEvent(	"filterwheel",
						"ZWO EFW failed EFWGetProperty()",
						NULL,
						kASCOM_Err_Success,
						lineBuff);
		}
		efwErrorCode	=	EFWClose(cFilterWheelDevNum);
		if (efwErrorCode == EFW_SUCCESS)
		{
			cSuccesfullCloses++;
		}
		else
		{
			CONSOLE_DEBUG_W_NUM("EFWClose() failed: efwErrorCode\t=", efwErrorCode);
			cCloseFailures++;
		}
	}
	else
	{
		//*	failed to open
		cOpenFailures++;
		cFilterWheelConnected	=	false;
		GetEFW_ErrorMsgString(efwErrorCode, lineBuff);
		LogEvent(	"filterwheel",
					"ZWO EFW failed to open",
					NULL,
					kASCOM_Err_Success,
					lineBuff);

		rulesFileOK	=	Check_udev_rulesFile(kEFW_rulesFile);
		if (rulesFileOK == false)
		{
			LogEvent(	"filterwheel",
						"ZWO EFW failed to open",
						NULL,
						kASCOM_Err_Success,
						"ZWO EFW Rules file does not appear to be installed properly");
		}

		CONSOLE_DEBUG("Failed to open filter wheel");
		CONSOLE_DEBUG_W_NUM("EFWOpen->efwErrorCode\t=", efwErrorCode);
		CONSOLE_DEBUG_W_NUM("cFilterWheelDevNum\t=", cFilterWheelDevNum);
		CONSOLE_DEBUG_W_STR("ErrMsg\t=", lineBuff);
	}
}


//*****************************************************************************
int	FilterwheelZWO::Read_CurrentFWstate(void)
{

	return(kFilterWheelState_OK);
}

//*****************************************************************************
//*	this returns a position starting with 0
//*	return -1 if unable to determine position
//*****************************************************************************
TYPE_ASCOM_STATUS	FilterwheelZWO::Read_CurrentFilterPositon(int *rtnCurrentPostion)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
EFW_ERROR_CODE		efwErrorCode;
int					myCurPosition;

	CONSOLE_DEBUG(__FUNCTION__);
	if (cFilterWheelIsOpen)
	{
		CONSOLE_DEBUG("Filter wheel is ALREADY OPEN!!!!!!!!!!!!!!!!!!!!");
	}
	myCurPosition		=	-99;
	efwErrorCode	=	EFWOpen(cFilterWheelDevNum);
	if (efwErrorCode == EFW_SUCCESS)
	{
		cSuccesfullOpens++;
		efwErrorCode	=	EFWGetPosition(cFilterWheelDevNum, &myCurPosition);
		if (efwErrorCode == EFW_SUCCESS)
		{
			cFilterWheelIsOpen	=	true;
			if (myCurPosition < 0)
			{
				CONSOLE_DEBUG("Filter wheel is moving");
	//			strcpy(errorMsg, "Filter wheel is moving");
				alpacaErrCode	=	kASCOM_Err_Success;
				myCurPosition	=	-1;
			}
			else
			{
				//*	the EFW filter wheel numbering starts at ZERO,
				cFilterWheelCurrPos	=	myCurPosition;
				strcpy(cFilterWheelCurrName, cFilterDef[cFilterWheelCurrPos].filterDesciption);
			}
		}
		else
		{
			CONSOLE_DEBUG_W_NUM("EFWGetPosition() Failed:efwErrorCode\t=", efwErrorCode);
			alpacaErrCode	=	kASCOM_Err_NotConnected;
		}
		efwErrorCode	=	EFWClose(cFilterWheelDevNum);
		if (efwErrorCode == EFW_SUCCESS)
		{
			cSuccesfullCloses++;
		}
		else
		{
			CONSOLE_DEBUG_W_NUM("EFWClose() failed: efwErrorCode\t=", efwErrorCode);
			cCloseFailures++;
		}
		cFilterWheelIsOpen	=	false;
	}
	else
	{
		CONSOLE_DEBUG_W_NUM("EFWOpen() failed:efwErrorCode\t=", efwErrorCode);

		cOpenFailures++;
		alpacaErrCode	=	kASCOM_Err_NotConnected;
	}
	if (rtnCurrentPostion != NULL)
	{
		*rtnCurrentPostion	=	myCurPosition;
	}

	return(alpacaErrCode);
}

//*****************************************************************************
//*	this accepts a position starting with 0
//*****************************************************************************
TYPE_ASCOM_STATUS	FilterwheelZWO::Set_CurrentFilterPositon(const int newPosition)
{
TYPE_ASCOM_STATUS	alpacaErrCode	=	kASCOM_Err_Success;
EFW_ERROR_CODE		efwErrorCode;

	CONSOLE_DEBUG(__FUNCTION__);
	//*	make sure the filter position is valid
	//*	the positions are number 0 -> [4,7]
	//*	we want to represent the slot numbers as they are on the devices
	if ((newPosition >= 0) && (newPosition < cFilterwheelInfo.slotNum))
	{
		//*	open the filter wheel
		efwErrorCode	=	EFWOpen(cFilterWheelDevNum);
		if (efwErrorCode == EFW_SUCCESS)
		{
			cSuccesfullOpens++;
			efwErrorCode	=	EFWSetPosition(cFilterWheelDevNum, newPosition);
			if (efwErrorCode == EFW_SUCCESS)
			{
				//*	this is the version that is +1 from what ZWO deals with
				cFilterWheelCurrPos	=	newPosition;
				strcpy(cFilterWheelCurrName, cFilterDef[newPosition].filterDesciption);
			}
			else
			{
				CONSOLE_DEBUG_W_NUM("EFWSetPosition->efwErrorCode\t=", efwErrorCode);
			}

			efwErrorCode	=	EFWClose(cFilterWheelDevNum);
			if (efwErrorCode == EFW_SUCCESS)
			{
				cSuccesfullCloses++;
			}
			else
			{
				CONSOLE_DEBUG_W_NUM("EFWClose->efwErrorCode\t=", efwErrorCode);
				cCloseFailures++;
			}
		}
		else
		{
			CONSOLE_DEBUG("Failed to open filter wheel");
			CONSOLE_DEBUG_W_NUM("EFWOpen->efwErrorCode\t=", efwErrorCode);

			cOpenFailures++;
		}
	}
	else
	{
		alpacaErrCode	=	kASCOM_Err_InvalidValue;
	}
	CONSOLE_DEBUG("exit");

	return(alpacaErrCode);
}


//*****************************************************************************
static	void	GetEFW_ErrorMsgString(EFW_ERROR_CODE errorCode, char *errorMsg)
{
	switch(errorCode)
	{
		case EFW_SUCCESS:				strcpy(errorMsg,	"EFW_SUCCESS");				break;
		case EFW_ERROR_INVALID_INDEX:	strcpy(errorMsg,	"EFW_ERROR_INVALID_INDEX");	break;
		case EFW_ERROR_INVALID_ID:		strcpy(errorMsg,	"EFW_ERROR_INVALID_ID");	break;
		case EFW_ERROR_INVALID_VALUE:	strcpy(errorMsg,	"EFW_ERROR_INVALID_VALUE");	break;
		case EFW_ERROR_REMOVED:			strcpy(errorMsg,	"EFW_ERROR_REMOVED");		break;
		case EFW_ERROR_MOVING:			strcpy(errorMsg,	"EFW_ERROR_MOVING");		break;
		case EFW_ERROR_ERROR_STATE:		strcpy(errorMsg,	"EFW_ERROR_ERROR_STATE");	break;
		case EFW_ERROR_GENERAL_ERROR:	strcpy(errorMsg,	"EFW_ERROR_GENERAL_ERROR");	break;
		case EFW_ERROR_NOT_SUPPORTED:	strcpy(errorMsg,	"EFW_ERROR_NOT_SUPPORTED");	break;
		case EFW_ERROR_CLOSED:			strcpy(errorMsg,	"EFW_ERROR_CLOSED");		break;
		default:						strcpy(errorMsg,	"unknown");					break;
	}
}


#endif // _ENABLE_FILTERWHEEL_

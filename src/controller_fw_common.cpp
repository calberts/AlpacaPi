//*****************************************************************************
//*		controller_fw_common.cpp		(c) 2021 by Mark Sproul
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
//*	Re-distributions of this source code must retain this copyright notice.
//*****************************************************************************
//*	Edit History
//*****************************************************************************
//*	May 28,	2021	<MLS> Created controller_fw_common.cpp
//*	May 29,	2021	<MLS> Added AlpacaGetFilterWheelStatus()
//*	Mar  1,	2022	<MLS> Added value string length checking to prevent crashes
//*	Jul 16,	2023	<MLS> Added name to AlpacaGetFilterWheelStartup()
//*****************************************************************************

//*	this file gets INCLUDED at the end of either controller_filterwheel.cpp OR controller_camera.cpp

#ifndef	_ALPACA_DEFS_H_
	#include	"alpaca_defs.h"
#endif

//*****************************************************************************
//http://newt16:6800/api/v1/filterwheel/0/names
//*****************************************************************************
bool	PARENT_CLASS::AlpacaGetFilterWheelStartup(void)
{
SJP_Parser_t	jsonParser;
bool			validData;
char			alpacaString[128];
int				jjj;
int				filterWheelIdx;
int				argStrLen;

//	CONSOLE_DEBUG_W_STR(__FUNCTION__, cWindowName);
//	CONSOLE_DEBUG_W_NUM("cAlpacaDevNum\t=", cAlpacaDevNum);
//	CONSOLE_DEBUG_W_NUM("cPort\t=", cPort);
	//*	Start by getting info about the filterwheel

#ifdef _PARENT_IS_FILTERWHEEL_
	//*	This is taken care of elsewhere
#else
bool			rtnValidData;
char			returnDataString[128];
	validData	=	AlpacaGetStringValue(	cDeviceAddress,
											cPort,
											cAlpacaDevNum,
											"filterwheel",
											"name",
											NULL,
											returnDataString,
											&rtnValidData);
	if (validData)
	{
		strcpy(cFilterWheelName, returnDataString);
		CONSOLE_DEBUG_W_STR("cFilterWheelName\t=", cFilterWheelName);
	}
	else
	{
		CONSOLE_DEBUG("AlpacaGetStringValue() returned error");
	}
#endif
	//===============================================================
	//*	get the filter wheel names
	SJP_Init(&jsonParser);
	sprintf(alpacaString,	"/api/v1/filterwheel/%d/names", cAlpacaDevNum);
	validData	=	GetJsonResponse(	&cDeviceAddress,
										cPort,
										alpacaString,
										NULL,
										&jsonParser);

	if (validData)
	{
		jjj	=	0;
		while (jjj<jsonParser.tokenCount_Data)
		{
//			CONSOLE_DEBUG_W_2STR("JSON:", jsonParser.dataList[jjj].keyword, jsonParser.dataList[jjj].valueString);
			argStrLen	=	strlen(jsonParser.dataList[jjj].valueString);
			if (strcasecmp(jsonParser.dataList[jjj].keyword, "DEVICE") == 0)
			{
				if (argStrLen < kCommonPropMaxStrLen)
				{
					strcpy(cFilterWheelName,	jsonParser.dataList[jjj].valueString);
				}
				else
				{
					CONSOLE_DEBUG_W_STR("argument too long", jsonParser.dataList[jjj].valueString);
					CONSOLE_DEBUG_W_NUM("argStrLen\t=", argStrLen);
				}
			}
			if (strcasecmp(jsonParser.dataList[jjj].keyword, "ARRAY") == 0)
			{

				filterWheelIdx	=	0;
				jjj++;
				while ((jjj<jsonParser.tokenCount_Data) &&
						(jsonParser.dataList[jjj].keyword[0] != ']'))
				{
					if (filterWheelIdx < kMaxFiltersPerWheel)
					{
						if (argStrLen < kMaxFWnameLen)
						{
							//*	save the filter name
							strcpy(cFilterWheelProp.Names[filterWheelIdx].FilterName, jsonParser.dataList[jjj].valueString);
						}
						else
						{
							CONSOLE_DEBUG_W_STR("argument too long", jsonParser.dataList[jjj].valueString);
							CONSOLE_DEBUG_W_NUM("argStrLen\t=", argStrLen);
						}

						filterWheelIdx++;
					}
					else
					{
						CONSOLE_ABORT("Exceeded max filter count");
					}
					jjj++;
				}
				cPositionCount	=	filterWheelIdx;
			}
			jjj++;
		}
		UpdateFilterWheelInfo();
	}
	else
	{
		CONSOLE_DEBUG("Read failure - filterwheel");
		cReadFailureCnt++;
	}
	//===============================================================
	//*	get the filter focus offsets
	SJP_Init(&jsonParser);
	sprintf(alpacaString,	"/api/v1/filterwheel/%d/focusoffsets", cAlpacaDevNum);
	validData	=	GetJsonResponse(	&cDeviceAddress,
										cPort,
										alpacaString,
										NULL,
										&jsonParser);

	if (validData)
	{
		jjj	=	0;
		while (jjj<jsonParser.tokenCount_Data)
		{
			CONSOLE_DEBUG_W_2STR("JSON:", jsonParser.dataList[jjj].keyword, jsonParser.dataList[jjj].valueString);
			argStrLen	=	strlen(jsonParser.dataList[jjj].valueString);
			if (strcasecmp(jsonParser.dataList[jjj].keyword, "ARRAY") == 0)
			{
				filterWheelIdx	=	0;
				jjj++;
				while ((jjj<jsonParser.tokenCount_Data) &&
						(jsonParser.dataList[jjj].keyword[0] != ']'))
				{
					if (filterWheelIdx < kMaxFiltersPerWheel)
					{
						//*	save the focus offset
						cFilterWheelProp.FocusOffsets[filterWheelIdx]	=	atoi(jsonParser.dataList[jjj].valueString);
						CONSOLE_DEBUG_W_NUM("FocusOffsets:", cFilterWheelProp.FocusOffsets[filterWheelIdx]);
						filterWheelIdx++;
					}
					else
					{
						CONSOLE_ABORT("Exceeded max filter count");
					}
					jjj++;
				}
			}
			jjj++;
		}
		UpdateFilterWheelInfo();
	}
	else
	{
		CONSOLE_DEBUG("Read failure - filterwheel");
		cReadFailureCnt++;
	}
//	int				FocusOffsets[kMaxFiltersPerWheel];	//	Focus offset of each filter in the wheel
	return(validData);
}


//*****************************************************************************
bool	PARENT_CLASS::AlpacaGetFilterWheelStatus(void)
{
bool	validData;
int		newFilterWheelPosition;
bool	rtnValidData;

//	CONSOLE_DEBUG_W_STR(__FUNCTION__, cWindowName);
#ifdef _PARENT_IS_FILTERWHEEL_
	validData	=	AlpacaGetCommonConnectedState("filterwheel");
#endif

	validData	=	AlpacaGetIntegerValue(	"filterwheel",
											"position",
											NULL,
											&newFilterWheelPosition,
											&rtnValidData);
//	CONSOLE_DEBUG_W_BOOL("rtnValidData\t=", rtnValidData);
	if (validData)
	{
		if (rtnValidData)
		{
	//		CONSOLE_DEBUG_W_NUM("rcvd newFilterWheelPosition\t=", newFilterWheelPosition);
			cOnLine	=	true;

			//*	alpaca/ascom uses filter wheel positions from 0 -> N-1
			if ((newFilterWheelPosition >= 0) && (newFilterWheelPosition < kMaxFiltersPerWheel))
			{
				cFilterWheelProp.Position	=	newFilterWheelPosition;
				cFilterWheelProp.IsMoving	=	false;
			}
			else if (newFilterWheelPosition == -1)
			{
				cFilterWheelProp.IsMoving	=	true;
			}
			UpdateFilterWheelPosition();
		}
	}
	else
	{
		CONSOLE_DEBUG("Failed to get filter wheel position");
	#ifdef _PARENT_IS_FILTERWHEEL_
		cOnLine	=	false;
	#endif

	#ifdef _PARENT_IS_CAMERA_
		CONSOLE_DEBUG_W_STR("This camera does NOT have a filterwheel", cWindowName);
		cHas_FilterWheel		=	false;
		UpdateFilterWheelPosition();
	#endif // _PARENT_IS_CAMERA_
	}
	SetWindowIPaddrInfo(NULL, cOnLine);
	return(validData);
}


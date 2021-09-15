//*****************************************************************************
//*		windowtab_filterwheel.cpp		(c) 2020 by Mark Sproul
//*
//*	Description:	C++ Client to talk to Alpaca devices
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
//*	May 27,	2021	<MLS> Created windowtab_filterwheel.cpp
//*	May 29,	2021	<MLS> Filterwheel display working correctly
//*****************************************************************************

#define _ENABLE_CTRL_FILTERWHEEL_

#if defined(_ENABLE_CTRL_FILTERWHEEL_)

#include	<math.h>

#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"

#include	"windowtab.h"
#include	"windowtab_filterwheel.h"
#include	"controller.h"


#define	kFilterCirleRadius	40


//**************************************************************************************
WindowTabFilterWheel::WindowTabFilterWheel(	const int	xSize,
								const int	ySize,
								CvScalar	backGrndColor,
								const char	*windowName)
	:WindowTab(xSize, ySize, backGrndColor, windowName)
{
int		iii;
//	CONSOLE_DEBUG(__FUNCTION__);

	cFilterWheelPropPtr	=	NULL;
	cPositionCount		=	0;
	for (iii=0; iii<kMaxFiltersPerWheel; iii++)
	{
		cFilterCirleCenterPt[iii].x	=	-10;
		cFilterCirleCenterPt[iii].y	=	-10;
	}

	SetupWindowControls();

}

//**************************************************************************************
// Destructor
//**************************************************************************************
WindowTabFilterWheel::~WindowTabFilterWheel(void)
{
//	CONSOLE_DEBUG(__FUNCTION__);
}



//**************************************************************************************
void	WindowTabFilterWheel::SetupWindowControls(void)
{
int		yLoc;

//	CONSOLE_DEBUG(__FUNCTION__);

	//------------------------------------------
	yLoc			=	cTabVertOffset;

	//------------------------------------------
	SetWidget(					kFilterWheel_Title,		0,			yLoc,		cWidth,		cTitleHeight);
	SetBGcolorFromWindowName(	kFilterWheel_Title);
	SetWidgetText(				kFilterWheel_Title, "AlpacaPi Filter Wheel");

	//*	setup the connected indicator
   	SetUpConnectedIndicator(kFilterWheel_Connected, yLoc);

	yLoc			+=	cTitleHeight;
	yLoc			+=	2;
	yLoc			+=	2;
	yLoc			+=	2;


	//------------------------------------------
	SetWidget(					kFilterWheel_Name,		0,			yLoc,		cWidth,		cTitleHeight);
//	SetBGcolorFromWindowName(	kFilterWheel_Name);
	SetWidgetText(				kFilterWheel_Name, "");
	yLoc			+=	cTitleHeight;
	yLoc			+=	2;
	yLoc			+=	2;


	//==========================================
	//*	create the filter wheel
	SetWidget(		kFilterWheel_FilterCircle,	0,		yLoc,		(cWidth),	(cWidth + 8));
	SetWidgetType(	kFilterWheel_FilterCircle, kWidgetType_Graphic);
	yLoc			+=	cWidth;




	yLoc			+=	2;


	SetAlpacaLogo(kFilterWheel_AlpacaLogo, kFilterWheel_LastCmdString);

	//=======================================================
	//*	IP address
	SetIPaddressBoxes(kFilterWheel_IPaddr, kFilterWheel_Readall, kFilterWheel_AlpacaDrvrVersion, -1);

}

//*****************************************************************************
void	WindowTabFilterWheel::ProcessButtonClick(const int buttonIdx)
{

	CONSOLE_DEBUG(__FUNCTION__);
	CONSOLE_DEBUG_W_NUM("buttonIdx\t=", buttonIdx);

	switch(buttonIdx)
	{
	}
}

//*****************************************************************************
void	WindowTabFilterWheel::ProcessDoubleClick(	const int	widgetIdx,
													const int	event,
													const int	xxx,
													const int	yyy,
													const int	flags)
{
int		iii;
int		deltaPixels_X;
int		deltaPixels_Y;
int		deltaPixels_Dist;
int		clickedFilter;
int		fwPosition;
char	dataString[128];
bool	validData;

//	CONSOLE_DEBUG(__FUNCTION__);
	switch(widgetIdx)
	{
		case kFilterWheel_FilterCircle:
			//*	see if we can figure out which filter was double clicked
			clickedFilter	=	-1;
			for (iii=0; iii < cPositionCount; iii++)
			{
				deltaPixels_X		=	xxx - cFilterCirleCenterPt[iii].x;
				deltaPixels_Y		=	yyy - cFilterCirleCenterPt[iii].y;
				deltaPixels_Dist	=	sqrt((deltaPixels_X * deltaPixels_X) + (deltaPixels_Y * deltaPixels_Y));
				if (deltaPixels_Dist <= kFilterCirleRadius)
				{
					//*	we have a valid clicked filter position
					CONSOLE_DEBUG_W_NUM("Clicked filter\t=",	iii);
					clickedFilter	=	iii;
				}
			}
			if (clickedFilter >= 0)
			{
				//*	we have a winner. Send the command to change to that filter
				fwPosition	=	clickedFilter + cFilterWheelPropPtr->Position;
				CONSOLE_DEBUG_W_NUM("fwPosition\t=",	fwPosition);
				if (fwPosition >= cPositionCount)
				{
					fwPosition	-=	cPositionCount;
				}
				CONSOLE_DEBUG_W_NUM("fwPosition\t=",	fwPosition);
				sprintf(dataString, "Position=%d", fwPosition);
				CONSOLE_DEBUG_W_STR("dataString\t=",	dataString);
				validData	=	AlpacaSendPutCmd(	"filterwheel", "position",	dataString);
				if (validData)
				{
					cFilterWheelPropPtr->IsMoving	=	true;
					ForceUpdate();
				}
				else
				{
					CONSOLE_DEBUG("Error setting filter wheel position");
				}
				DisplayLastAlpacaCommand();
			}
			break;
	}
}

//**************************************************************************************
void	WindowTabFilterWheel::DrawGraphWidget(IplImage *openCV_Image, const int widgetIdx)
{
	switch(widgetIdx)
	{
		case kFilterWheel_FilterCircle:
			DrawFilterWheel(openCV_Image, &cWidgetList[widgetIdx]);
			break;

		default:
			CONSOLE_DEBUG_W_NUM("widgetIdx\t",	widgetIdx);
			break;
	}
}


//**************************************************************************************
void	WindowTabFilterWheel::DrawFilterWheel(IplImage *openCV_Image, TYPE_WIDGET *theWidget)
{
CvRect			myCVrect;
CvPoint			myCVcenter;
int				radius1;
double			degrees;
double			radians;
CvPoint			pt1;
char			textString[64];
char			myFilterName[32];
int				filterPosition;		//*	0	= 12 o'clock
CvScalar		fillColor;
CvScalar		textColor;
CvSize			textSize;
int				baseLine;
double			deltaDegrees;
int				myFilterOffset;

	myCVrect.x		=	theWidget->left;
	myCVrect.y		=	theWidget->top;
	myCVrect.width	=	theWidget->width;
	myCVrect.height	=	theWidget->height;


	cvRectangleR(	openCV_Image,
					myCVrect,
					theWidget->bgColor,			//	CvScalar color,
					CV_FILLED,					//	int thickness CV_DEFAULT(1),
					8,							//	int line_type CV_DEFAULT(8),
					0);							//	int shift CV_DEFAULT(0));


	cvRectangleR(	openCV_Image,
					myCVrect,
					theWidget->borderColor,		//	CvScalar color,
					1,							//	int thickness CV_DEFAULT(1),
					8,							//	int line_type CV_DEFAULT(8),
					0);							//	int shift CV_DEFAULT(0));

	myCVcenter.x	=	myCVrect.x + (myCVrect.width / 2);
	myCVcenter.y	=	myCVrect.y + (myCVrect.height / 2);

	cvCircle(	openCV_Image,
				myCVcenter,
				((myCVrect.width / 2) -2),			//*	radius
				theWidget->borderColor,
				1,									//	int thickness CV_DEFAULT(1),
				8,									//	int line_type CV_DEFAULT(8),
				0);									//	int shift CV_DEFAULT(0));

	if (cFilterWheelPropPtr != NULL)
	{
		if (cFilterWheelPropPtr->IsMoving)
		{
			strcpy(textString, "Moving");
		}
		else
		{
			strcpy(textString, "Double click to change filter");
		}
		cvGetTextSize(	textString,
						&gTextFont[kFont_Medium],
						&textSize,
						&baseLine);

		pt1.x		=	myCVcenter.x;
		pt1.y		=	myCVcenter.y;
		pt1.x		-=	(textSize.width / 2);
		textColor	=	CV_RGB(255, 255, 255);
		cvPutText(	openCV_Image,
					textString,
					pt1,
					&gTextFont[kFont_Medium],
					textColor
					);
	}


	//*	do we have a valid count??
	if (cPositionCount > 0)
	{
		//*	now draw a circle for each filter
		deltaDegrees	=	360.0 / cPositionCount;
		radius1			=	(myCVrect.width / 2) - 45;
		for (filterPosition=0; filterPosition < cPositionCount; filterPosition++)
		{
			//*	default text color
			textColor		=	CV_RGB(255, 255, 255);
			myFilterOffset	=	0;
			if (cFilterWheelPropPtr != NULL)
			{
				myFilterOffset	=	filterPosition + cFilterWheelPropPtr->Position;
				//*	figure out the index into the array for the one we are currently drawing
				if (myFilterOffset >= cPositionCount)
				{
					//*	handle wrap around
					myFilterOffset	-=	cPositionCount;
				}
				strcpy(myFilterName, cFilterWheelPropPtr->Names[myFilterOffset].FilterName);

				//*	figure out what color to make the background
				fillColor		=	CV_RGB(0, 0, 0);
				if (strncasecmp(myFilterName, "red", 3) == 0)
				{
					fillColor		=	CV_RGB(255, 0, 0);
				}
				else if (strncasecmp(myFilterName, "green", 5) == 0)
				{
					fillColor		=	CV_RGB(0, 255, 0);
					textColor		=	CV_RGB(0, 0, 0);
				}
				else if (strncasecmp(myFilterName, "blue", 4) == 0)
				{
					fillColor		=	CV_RGB(0, 0, 255);
				}
				//	So, Sii=R, Ha=G, Oiii=B.
				else if ((strncasecmp(myFilterName, "Sii", 3) == 0) || (strncasecmp(myFilterName, "S2", 2) == 0))
				{
					fillColor		=	CV_RGB(255,85,85);
				}
				else if (strncasecmp(myFilterName, "Ha", 2) == 0)
				{
					fillColor		=	CV_RGB(151,253,151);
					textColor		=	CV_RGB(0, 0, 0);
				}
				else if (strncasecmp(myFilterName, "Oiii", 4) == 0)
				{
					fillColor		=	CV_RGB(127,216,250);
					textColor		=	CV_RGB(0, 0, 0);
				}

			}
			else
			{
				fillColor	=	CV_RGB(128, 128, 128);
				sprintf(myFilterName, "-%d-", (myFilterOffset + 1));
			}

			degrees	=	-90.0 + (filterPosition * deltaDegrees);
			radians	=	degrees * M_PI / 180.0;
			pt1.x	=	myCVcenter.x + (cos(radians) * radius1);
			pt1.y	=	myCVcenter.y + (sin(radians) * radius1);

			//*	save the center of the filter circle
			//*	this is for the double click routine
			cFilterCirleCenterPt[filterPosition]	=	pt1;

			cvCircle(	openCV_Image,
						pt1,
						kFilterCirleRadius,						//*	radius
						fillColor,
						CV_FILLED,							//	int thickness CV_DEFAULT(1),
						8,									//	int line_type CV_DEFAULT(8),
						0);									//	int shift CV_DEFAULT(0));

			cvCircle(	openCV_Image,
						pt1,
						kFilterCirleRadius,
						theWidget->borderColor,
						1,									//	int thickness CV_DEFAULT(1),
						8,									//	int line_type CV_DEFAULT(8),
						0);									//	int shift CV_DEFAULT(0));

			//*	make position zero distinct
			if (filterPosition == 0)
			{
				cvCircle(	openCV_Image,
							pt1,
							44,
							theWidget->borderColor,
							4,									//	int thickness CV_DEFAULT(1),
							8,									//	int line_type CV_DEFAULT(8),
							0);									//	int shift CV_DEFAULT(0));
			}
			//*	print the name of the filter in the circle
			cvGetTextSize(	myFilterName,
							&gTextFont[kFont_Medium],
							&textSize,
							&baseLine);


			pt1.x	-=	(textSize.width / 2);
			pt1.y	+=	5;
			cvPutText(	openCV_Image,
						myFilterName,
						pt1,
						&gTextFont[kFont_Medium],
						textColor
						);

			//*	now print the number of the filter above the name
			sprintf(textString, "-%d-", (myFilterOffset + 1));
			cvGetTextSize(	textString,
							&gTextFont[kFont_Medium],
							&textSize,
							&baseLine);


			pt1.x	=	myCVcenter.x + (cos(radians) * radius1);
			pt1.y	=	myCVcenter.y + (sin(radians) * radius1);
			pt1.x	-=	(textSize.width / 2);
			pt1.y	-=	15;
			cvPutText(	openCV_Image,
						textString,
						pt1,
						&gTextFont[kFont_Medium],
						textColor
						);

			degrees	+=	deltaDegrees;
		}
	}
}


//**************************************************************************************
void	WindowTabFilterWheel::SetPositonCount(int positionCount)
{
	CONSOLE_DEBUG(__FUNCTION__);
	CONSOLE_DEBUG_W_NUM("positionCount\t=", positionCount);
	cPositionCount	=	positionCount;
}

//**************************************************************************************
void	WindowTabFilterWheel::SetFilterWheelPropPtr(TYPE_FilterWheelProperties *fwProperties)
{
	CONSOLE_DEBUG(__FUNCTION__);

	cFilterWheelPropPtr	=	fwProperties;
}


#endif // _ENABLE_CTRL_FILTERWHEEL_


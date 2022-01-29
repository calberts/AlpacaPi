//*****************************************************************************
//*		windowtab_STsettings.cpp		(c) 2021 by Mark Sproul
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
//*	Re-distributions of this source code must retain this copyright notice.
//*****************************************************************************
//*	Edit History
//*****************************************************************************
//*	Apr 21,	2020	<MLS> Created windowtab_STsettings.cpp
//*	Jul  5,	2021	<MLS> Added star count table to settings tab
//*	Jul 17,	2021	<MLS> Added double click to SkyTravelSettings window
//*	Sep 12,	2021	<MLS> Added settings for line widths
//*	Sep 12,	2021	<MLS> Added reset-to-default button
//*	Oct 28,	2021	<MLS> Added day/night sky option, blue sky during the day
//*	Dec 13,	2021	<MLS> Added buttons to run Startup and Shutdown scripts
//*	Dec 13,	2021	<MLS> Added RunShellScript_Thead() and RunShellScript()
//*	Dec 31,	2021	<MLS> Added Asteroid count display to settings window
//*****************************************************************************

#include	<pthread.h>

#include	"controller.h"

#define _ENABLE_CONSOLE_DEBUG_
#include	"ConsoleDebug.h"

#include	"alpaca_defs.h"
#include	"helper_functions.h"
#include	"alpaca_defs.h"
#include	"windowtab.h"
#include	"windowtab_STsettings.h"
#include	"controller.h"
#include	"controller_skytravel.h"
#include	"observatory_settings.h"
#include	"controller_starlist.h"
#include	"controller_constList.h"
#include	"controller_GaiaRemote.h"

#include	"SkyStruc.h"
#include	"AsteroidData.h"

#define	kSkyT_SettingsHeight	100

//**************************************************************************************
WindowTabSTsettings::WindowTabSTsettings(	const int	xSize,
								const int	ySize,
								CvScalar	backGrndColor,
								const char	*windowName)
	:WindowTab(xSize, ySize, backGrndColor, windowName)
{
	CONSOLE_DEBUG(__FUNCTION__);


	SetupWindowControls();

	UpdateSettings();
}

//**************************************************************************************
// Destructor
//**************************************************************************************
WindowTabSTsettings::~WindowTabSTsettings(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
}



//**************************************************************************************
void	WindowTabSTsettings::SetupWindowControls(void)
{
int		yLoc;
int		xLoc;
int		xLoc2;
int		yLoc2;
int		saveXloc;
int		saveYloc;
int		labelWidth;
int		valueWitdth1;
int		valueWitdth2;
int		iii;
char	textString[64];
int		myBoxHeight;
int		radioBtnWidth;

	CONSOLE_DEBUG(__FUNCTION__);

	//------------------------------------------
	yLoc			=	cTabVertOffset;

	//------------------------------------------
	SetWidget(kSkyT_Settings_Title,		0,			yLoc,		cWidth,		cTitleHeight);
	SetWidgetText(kSkyT_Settings_Title, "SkyTravel settings");
	SetBGcolorFromWindowName(kSkyT_Settings_Title);
	yLoc			+=	cTitleHeight;
	yLoc			+=	2;
	yLoc			+=	2;
	yLoc			+=	2;


	//----------------------------------------------------------------------
	labelWidth		=	150;
	valueWitdth1	=	100;
	valueWitdth2	=	150;
	xLoc			=	5;

	SetWidget(			kSkyT_Settings_LatLonTitle,	xLoc,	yLoc,	(labelWidth + valueWitdth1 + valueWitdth2),	cTitleHeight);
	SetWidgetFont(		kSkyT_Settings_LatLonTitle,	kFont_Medium);
	SetWidgetText(		kSkyT_Settings_LatLonTitle,	"Site Information");
	SetWidgetBorder(	kSkyT_Settings_LatLonTitle, false);
	SetWidgetTextColor(kSkyT_Settings_LatLonTitle,	CV_RGB(255,	255,	255));
	yLoc			+=	cTitleHeight;
	yLoc			+=	2;

	saveXloc		=	5 + (labelWidth + valueWitdth1 + valueWitdth2);
	saveXloc		+=	15;

	//--------------------------------------------------------------------------
	iii	=	kSkyT_Settings_LatLable;
	while (iii < (kSkyT_Settings_SiteInfoOutline -1))
	{
		xLoc		=	5;
		SetWidget(			iii,	xLoc,			yLoc,		labelWidth,		cTitleHeight);
		SetWidgetFont(		iii,	kFont_Medium);
		SetWidgetJustification(	iii,	kJustification_Left);
		iii++;
		xLoc		+=	labelWidth;
		xLoc		+=	3;

		SetWidget(			iii,	xLoc,			yLoc,		valueWitdth1,		cTitleHeight);
		SetWidgetFont(		iii,	kFont_Medium);
		iii++;

		xLoc		+=	valueWitdth1;
		xLoc		+=	3;

		SetWidget(			iii,	xLoc,			yLoc,		valueWitdth2,		cTitleHeight);
		SetWidgetFont(		iii,	kFont_Medium);
		iii++;


		yLoc			+=	cTitleHeight;
		yLoc			+=	2;
	}
	SetWidgetText(		kSkyT_Settings_LatLable,	"Latitude");
	SetWidgetText(		kSkyT_Settings_LonLable,	"Longitude");
	SetWidgetText(		kSkyT_Settings_ElevLable,	"Elevation");
	SetWidgetText(		kSkyT_Settings_TimeZoneLbl,	"UTC/Time Zone");



	xLoc			=	5;
	SetWidget(			kSkyT_Settings_ObsSettingsText,	xLoc,	yLoc,	(labelWidth + valueWitdth1 + valueWitdth2),	cTitleHeight);
	SetWidgetFont(		kSkyT_Settings_ObsSettingsText,	kFont_TextList);
	SetWidgetText(		kSkyT_Settings_ObsSettingsText,	"Edit observatorysettings.txt to change these values");
	SetWidgetBorder(	kSkyT_Settings_ObsSettingsText, false);
	SetWidgetTextColor(kSkyT_Settings_ObsSettingsText,	CV_RGB(255,	255,	255));
	yLoc			+=	cTitleHeight;
	yLoc			+=	2;

	SetWidgetOutlineBox(kSkyT_Settings_SiteInfoOutline, kSkyT_Settings_LatLonTitle, (kSkyT_Settings_SiteInfoOutline -1));

	//-----------------------------------------------------
	if (gObseratorySettings.ValidLatLon)
	{
		SetWidgetNumber6F(kSkyT_Settings_LatValue1, gObseratorySettings.Latitude);
		SetWidgetNumber6F(kSkyT_Settings_LonValue1, gObseratorySettings.Longitude);

		FormatHHMMSS(gObseratorySettings.Latitude, textString, true);
		SetWidgetText(		kSkyT_Settings_LatValue2,	textString);

		FormatHHMMSS(gObseratorySettings.Longitude, textString, true);
		SetWidgetText(		kSkyT_Settings_LonValue2,	textString);

		SetWidgetText(		kSkyT_Settings_TimeZoneTxt,	gObseratorySettings.TimeZone);
		SetWidgetNumber(	kSkyT_Settings_UTCoffset,	gObseratorySettings.UTCoffset);

		sprintf(textString, "%1.1f m", gObseratorySettings.Elevation_m);
		SetWidgetText(		kSkyT_Settings_ElevValueMeters,	textString);

		sprintf(textString, "%1.0f ft", gObseratorySettings.Elevation_ft);
		SetWidgetText(		kSkyT_Settings_ElevValueFeet,	textString);
	}

	//--------------------------------------------------------------------------
	yLoc	+=	12;
	xLoc	=	5;
	iii		=	kSkyT_Settings_EarthLable;
	while (iii < kSkyT_Settings_EarthOutline)
	{
		if (iii == kSkyT_Settings_DayNightSky)
		{
			//*	leave some extra space
			yLoc			+=	cRadioBtnHt;
		}
		SetWidget(		iii,	xLoc,			yLoc,		labelWidth,		cRadioBtnHt);
		SetWidgetFont(	iii,	kFont_RadioBtn);
		SetWidgetType(	iii,	kWidgetType_RadioButton);

		yLoc			+=	cRadioBtnHt;
		yLoc			+=	2;
		iii++;
	}
	SetWidgetType(		kSkyT_Settings_EarthLable,	kWidgetType_Text);
	SetWidgetText(		kSkyT_Settings_EarthLable,		"Earth Display");
	SetWidgetText(		kSkyT_Settings_EarthThin,		"Thin Lines");
	SetWidgetText(		kSkyT_Settings_EarthThick,		"Thick Lines");
	SetWidgetText(		kSkyT_Settings_EarthSolidBrn,	"Solid Brown");
	SetWidgetText(		kSkyT_Settings_EarthSolidGrn,	"Solid Green");

	SetWidgetText(		kSkyT_Settings_DayNightSky,		"Day/Night sky");
	SetWidgetType(		kSkyT_Settings_DayNightSky,		kWidgetType_CheckBox);


	SetWidgetOutlineBox(kSkyT_Settings_EarthOutline, kSkyT_Settings_EarthLable, (kSkyT_Settings_EarthOutline -1));


	//-----------------------------------------------------
	yLoc	+=	12;
	xLoc	=	5;
	iii		=	kSkyT_Settings_GridLable;
	while (iii < kSkyT_Settings_GridOutline)
	{
		SetWidget(		iii,	xLoc,			yLoc,		labelWidth,		cRadioBtnHt);
		SetWidgetFont(	iii,	kFont_RadioBtn);
		SetWidgetType(	iii,	kWidgetType_RadioButton);

		yLoc			+=	cRadioBtnHt;
		yLoc			+=	2;
		iii++;
	}
	SetWidgetType(		kSkyT_Settings_GridLable,	kWidgetType_Text);
	SetWidgetText(		kSkyT_Settings_GridLable,	"Grid Lines");
	SetWidgetText(		kSkyT_Settings_GridSolid,	"Solid Lines");
	SetWidgetText(		kSkyT_Settings_GridDashed,	"Dashed Lines");
	SetWidgetOutlineBox(kSkyT_Settings_GridOutline, kSkyT_Settings_GridLable, (kSkyT_Settings_GridOutline -1));

	yLoc			+=	cRadioBtnHt;
	yLoc			+=	2;
	yLoc			+=	2;

	//-----------------------------------------------------
	//*	Line Width settings
	labelWidth	=	225;
	SetWidget(		kSkyT_Settings_LineWidthTitle,	xLoc,		yLoc,	labelWidth + (4 * cRadioBtnHt),	cBoxHeight);
	SetWidgetFont(	kSkyT_Settings_LineWidthTitle,	kFont_Medium);
	SetWidgetType(	kSkyT_Settings_LineWidthTitle,	kWidgetType_Text);
	SetWidgetText(	kSkyT_Settings_LineWidthTitle,	"Line Widths");
	yLoc			+=	cBoxHeight;
	yLoc			+=	2;

	iii			=	kSkyT_Settings_LineW_Const;
	while (iii < kSkyT_Settings_LineW_BoxOutline)
	{
		xLoc2	=	xLoc;
		SetWidget(		iii,	xLoc2,			yLoc,		labelWidth,		cRadioBtnHt);
		SetWidgetFont(	iii,	kFont_Medium);
		SetWidgetType(	iii,	kWidgetType_Text);
		SetWidgetJustification(iii, kJustification_Left);

		xLoc2	+=	labelWidth;
		iii++;

		SetWidget(		iii,	xLoc2,			yLoc,		cRadioBtnHt * 2,		cRadioBtnHt);
		SetWidgetFont(	iii,	kFont_Medium);
		SetWidgetType(	iii,	kWidgetType_RadioButton);
		SetWidgetText(	iii,	"1");
		xLoc2	+=	cRadioBtnHt * 2;
		iii++;

		SetWidget(		iii,	xLoc2,			yLoc,		cRadioBtnHt * 2,		cRadioBtnHt);
		SetWidgetFont(	iii,	kFont_Medium);
		SetWidgetType(	iii,	kWidgetType_RadioButton);
		SetWidgetText(	iii,	"2");
		iii++;

		yLoc			+=	cRadioBtnHt;
		yLoc			+=	2;
	}
	SetWidgetOutlineBox(	kSkyT_Settings_LineW_BoxOutline,
							kSkyT_Settings_LineWidthTitle,
							(kSkyT_Settings_LineW_BoxOutline -1));
	SetWidgetText(	kSkyT_Settings_LineW_Const,			"Constellations");
	SetWidgetText(	kSkyT_Settings_LineW_ConstOutlines,	"Constellation Outlines");
	SetWidgetText(	kSkyT_Settings_LineW_Grid,			"Grid Lines");

//	yLoc			+=	cBoxHeight;
	yLoc			+=	2;


	SetWidget(			kSkyT_Settings_ResetToDefault,	xLoc,		yLoc,	labelWidth + (4 * cRadioBtnHt),	cBoxHeight);
	SetWidgetFont(		kSkyT_Settings_ResetToDefault,	kFont_Medium);
	SetWidgetType(		kSkyT_Settings_ResetToDefault,	kWidgetType_Button);
	SetWidgetBGColor(	kSkyT_Settings_ResetToDefault,	CV_RGB(255,	255,	255));
	SetWidgetText(		kSkyT_Settings_ResetToDefault,	"Reset to default");



	//-----------------------------------------------------
	//*	data value settings
	yLoc		=	cTabVertOffset;
	yLoc		+=	cTitleHeight;
	yLoc		+=	6;
//	xLoc		=	cWidth / 2;
	xLoc		=	saveXloc;
	labelWidth	=	200;

	SetWidget(		kSkyT_Settings_DataTitle,	xLoc,		yLoc,	(labelWidth + 75),		cBoxHeight);
	SetWidgetFont(	kSkyT_Settings_DataTitle,	kFont_Medium);
	SetWidgetType(	kSkyT_Settings_DataTitle,	kWidgetType_Text);
	SetWidgetText(	kSkyT_Settings_DataTitle,	"Star Data counts");
	yLoc			+=	cBoxHeight;
	yLoc			+=	2;

	saveXloc		=	xLoc + (labelWidth + 75);
	saveXloc		+=	15;

	iii			=	kSkyT_Settings_OriginalData_txt;
	while (iii < kSkyT_Settings_DataHelpText)
	{
		SetWidget(		iii,	xLoc,			yLoc,		labelWidth,		cBoxHeight);
		SetWidgetFont(	iii,	kFont_Medium);
		SetWidgetType(	iii,	kWidgetType_Text);
		SetWidgetJustification(iii, kJustification_Left);
		iii++;

		SetWidget(		iii,	(xLoc + labelWidth),	yLoc,		75,		cBoxHeight);
		SetWidgetFont(	iii,	kFont_Medium);
		SetWidgetType(	iii,	kWidgetType_Text);
		iii++;


		yLoc			+=	cBoxHeight;
		yLoc			+=	2;
	}
	SetWidget(		kSkyT_Settings_DataHelpText,	xLoc,			yLoc,		labelWidth + 75,	cBoxHeight);
	SetWidgetFont(	kSkyT_Settings_DataHelpText,	kFont_Medium);
	SetWidgetType(	kSkyT_Settings_DataHelpText,	kWidgetType_Text);
	SetWidgetJustification(kSkyT_Settings_DataHelpText, kJustification_Center);
	SetWidgetText(			kSkyT_Settings_DataHelpText,	"Double click on green");
	SetWidgetTextColor(		kSkyT_Settings_DataHelpText,	CV_RGB(0,	255,	0));

	yLoc			+=	cBoxHeight;
	yLoc			+=	2;


	SetWidgetOutlineBox(	kSkyT_Settings_DataOutLine,
							kSkyT_Settings_DataTitle,
							(kSkyT_Settings_DataOutLine -1));

	//*	set the star counts
	SetWidgetText(		kSkyT_Settings_OriginalData_txt,	"OrigDB star count");
	SetWidgetNumber(	kSkyT_Settings_OriginalData_cnt,	gStarCount);


	SetWidgetText(		kSkyT_Settings_DataAAVSO_txt,		"AAVSO Alert count");
	SetWidgetNumber(	kSkyT_Settings_DataAAVSO_cnt,		gAAVSOalertsCnt);

	SetWidgetText(		kSkyT_Settings_DataDRAPER_txt,		"DRAPER Data count");
	SetWidgetNumber(	kSkyT_Settings_DataDRAPER_cnt,		gDraperObjectCount);

	SetWidgetText(		kSkyT_Settings_DataHipparcos_txt,	"Hipparcos Data count");
	SetWidgetNumber(	kSkyT_Settings_DataHipparcos_cnt,	gHipObjectCount);

	SetWidgetText(		kSkyT_Settings_DataHYG_txt,			"HYG Data count");
	SetWidgetNumber(	kSkyT_Settings_DataHYG_cnt,			gHYGObjectCount);

	SetWidgetText(		kSkyT_Settings_DataMessier_txt,		"Messier Data count");
	SetWidgetNumber(	kSkyT_Settings_DataMessier_cnt,		gMessierObjectCount);

	SetWidgetText(		kSkyT_Settings_DataNGC_txt,			"NGC Data count");
	SetWidgetNumber(	kSkyT_Settings_DataNGC_cnt,			gNGCobjectCount);

	SetWidgetText(		kSkyT_Settings_DataYALE_txt,		"YALE Data count");
	SetWidgetNumber(	kSkyT_Settings_DataYALE_cnt,		gYaleStarCount);

	SetWidgetText(		kSkyT_Settings_Constellations_txt,	"Constellations");
	SetWidgetNumber(	kSkyT_Settings_Constellations_cnt,	gConstVectorCnt);

	SetWidgetText(		kSkyT_Settings_ConstOutLines_txt,	"Outlines");
	SetWidgetNumber(	kSkyT_Settings_ConstOutLines_cnt,	gConstOutlineCount);

	SetWidgetText(		kSkyT_Settings_Gaia_txt,			"Gaia Data count");
	SetWidgetNumber(	kSkyT_Settings_Gaia_cnt,			gGaiaObjectCnt);

#ifdef _ENABLE_ASTERIODS_
	strcpy(textString, "Asteroids");
	if (strlen(gAsteroidDatbase) > 0)
	{
		strcat(textString, " (");
		strcat(textString, gAsteroidDatbase);
		strcat(textString, ")");
	}
#else
	strcpy(textString, "Asteroids disabled");
#endif
	SetWidgetText(		kSkyT_Settings_Asteroids_txt,		textString);
	SetWidgetNumber(	kSkyT_Settings_Asteroids_cnt,		gAsteroidCnt);



	//*	set the ones we can double click to green

	SetWidgetTextColor(	kSkyT_Settings_OriginalData_txt,	CV_RGB(0,	255,	0));
	SetWidgetTextColor(	kSkyT_Settings_OriginalData_cnt,	CV_RGB(0,	255,	0));

	SetWidgetTextColor(	kSkyT_Settings_DataAAVSO_txt,		CV_RGB(0,	255,	0));
	SetWidgetTextColor(	kSkyT_Settings_DataAAVSO_cnt,		CV_RGB(0,	255,	0));

	SetWidgetTextColor(	kSkyT_Settings_DataMessier_txt,		CV_RGB(0,	255,	0));
	SetWidgetTextColor(	kSkyT_Settings_DataMessier_cnt,		CV_RGB(0,	255,	0));

	SetWidgetTextColor(	kSkyT_Settings_DataNGC_txt,			CV_RGB(0,	255,	0));
	SetWidgetTextColor(	kSkyT_Settings_DataNGC_cnt,			CV_RGB(0,	255,	0));

	SetWidgetTextColor(	kSkyT_Settings_DataYALE_txt,		CV_RGB(0,	255,	0));
	SetWidgetTextColor(	kSkyT_Settings_DataYALE_cnt,		CV_RGB(0,	255,	0));

	SetWidgetTextColor(	kSkyT_Settings_Constellations_txt,	CV_RGB(0,	255,	0));
	SetWidgetTextColor(	kSkyT_Settings_Constellations_cnt,	CV_RGB(0,	255,	0));

#ifdef _ENABLE_REMOTE_GAIA_
	SetWidgetTextColor(	kSkyT_Settings_Gaia_txt,			CV_RGB(0,	255,	0));
	SetWidgetTextColor(	kSkyT_Settings_Gaia_cnt,			CV_RGB(0,	255,	0));
#endif // _ENABLE_REMOTE_GAIA_

	//-------------------------------------------------------------------------
	yLoc		=	cTabVertOffset;
	yLoc		+=	cTitleHeight;
	yLoc		+=	6;
//	xLoc		=	cWidth / 2;
	xLoc		=	saveXloc;

	//*	set up OBAFGKM color box
//	xLoc			=	5;
	myBoxHeight		=	75;
	SetWidget(			kSkyT_Settings_OBAFGKM,	xLoc,	yLoc,	450,	myBoxHeight);
	SetWidgetType(		kSkyT_Settings_OBAFGKM,	kWidgetType_Graphic);
	SetWidgetTextColor(	kSkyT_Settings_OBAFGKM,		CV_RGB(0,	0,	0));

	yLoc			+=	myBoxHeight;
	yLoc			+=	2;

	//-------------------------------------------------------------------------
	SetWidget(			kSkyT_Settings_DispMag,	xLoc,	yLoc,	450,	cSmallBtnHt);
	SetWidgetType(		kSkyT_Settings_DispMag,	kWidgetType_CheckBox);
	SetWidgetFont(		kSkyT_Settings_DispMag,	kFont_Medium);
	SetWidgetText(		kSkyT_Settings_DispMag,		"Display Star Magnitude");

	yLoc			+=	cSmallBtnHt;
	yLoc			+=	2;

	//-------------------------------------------------------------------------
	SetWidget(			kSkyT_Settings_DispSpectral,	xLoc,	yLoc,	450,	cSmallBtnHt);
	SetWidgetType(		kSkyT_Settings_DispSpectral,	kWidgetType_CheckBox);
	SetWidgetFont(		kSkyT_Settings_DispSpectral,	kFont_Medium);
	SetWidgetText(		kSkyT_Settings_DispSpectral,	"Display Star Spectral Class");

	yLoc			+=	cSmallBtnHt;
	yLoc			+=	2;
	saveYloc		=	yLoc;
	radioBtnWidth	=	300;
	for (iii=kSkyT_Settings_DispDynMagnitude; iii<= kSkyT_Settings_DispAllMagnitude; iii++)
	{
		SetWidget(			iii,	xLoc,	yLoc,	radioBtnWidth,	cSmallBtnHt);
		SetWidgetType(		iii,	kWidgetType_RadioButton);
		SetWidgetFont(		iii,	kFont_Medium);
//		SetWidgetBorder(	iii,	true);

		if (iii == kSkyT_Settings_DispSpecifiedMagnitude)
		{
			saveYloc	=	yLoc;
		}

		yLoc			+=	cSmallBtnHt;
		yLoc			+=	2;
	}
	SetWidgetText(		kSkyT_Settings_DispDynMagnitude,		"Display dynamic magnitude");
	SetWidgetText(		kSkyT_Settings_DispSpecifiedMagnitude,	"Display specified magnitude");
	SetWidgetText(		kSkyT_Settings_DispAllMagnitude,		"Display all magnitudes");

	SetWidget(			kSkyT_Settings_MagnitudeLimit,	(xLoc + radioBtnWidth),
														saveYloc,	100,	cSmallBtnHt);
	SetWidgetType(		kSkyT_Settings_MagnitudeLimit,	kWidgetType_Text);
	SetWidgetFont(		kSkyT_Settings_MagnitudeLimit,	kFont_Medium);

	xLoc2		=	xLoc + radioBtnWidth + 100;
	xLoc2		+=	2;
	yLoc2		=	saveYloc - (cSmallBtnHt / 2);

	SetWidget(			kSkyT_Settings_MagUpArrow,		xLoc2,	yLoc2,	cSmallBtnHt,	cSmallBtnHt);
	yLoc2		+=	cSmallBtnHt;
	SetWidget(			kSkyT_Settings_MagDownArrow,	xLoc2,	yLoc2,	cSmallBtnHt,	cSmallBtnHt);

	SetWidgetBorder(	kSkyT_Settings_MagUpArrow, true);

	SetWidgetIcon(kSkyT_Settings_MagUpArrow,	kIcon_UpArrow);
	SetWidgetIcon(kSkyT_Settings_MagDownArrow,	kIcon_DownArrow);
	SetWidgetBorder(	kSkyT_Settings_MagUpArrow, true);
	SetWidgetBorder(	kSkyT_Settings_MagDownArrow, true);


	SetWidgetOutlineBox(	kSkyT_Settings_StarOutLine,
							kSkyT_Settings_OBAFGKM,
							(kSkyT_Settings_StarOutLine -1));


	yLoc			+=	cSmallBtnHt;
	yLoc			+=	2;

	for (iii=kSkyT_Settings_RunStartup; iii<=kSkyT_Settings_RunShutdown; iii++)
	{
		SetWidget(			iii,	xLoc,	yLoc,	radioBtnWidth,	cSmallBtnHt);
		SetWidgetType(		iii,	kWidgetType_Button);
		SetWidgetFont(		iii,	kFont_Medium);
		SetWidgetBGColor(	iii,	CV_RGB(255,	255,	255));
		SetWidgetTextColor(	iii,	CV_RGB(0,	0,	0));

		yLoc			+=	cSmallBtnHt;
		yLoc			+=	2;
	}
	SetWidgetText(		kSkyT_Settings_RunStartup,	"Run Startup script (startup.sh)");
	SetWidgetText(		kSkyT_Settings_RunShutdown,	"Run Shutdown script (shutdown.sh)");


	SetAlpacaLogoBottomCorner(kSkyT_Settings_AlpacaLogo);

}

//**************************************************************************************
//*	gets called when the window tab changes
//**************************************************************************************
void	WindowTabSTsettings::ActivateWindow(void)
{
	CONSOLE_DEBUG(__FUNCTION__);
	UpdateSettings();
}

//**************************************************************************************
void	WindowTabSTsettings::DrawOBAFGKM(IplImage *openCV_Image, TYPE_WIDGET *theWidget)
{
CvRect			myCVrect;
CvPoint			myCVcenter;
int				radius1;
CvPoint			pt1;
int				iii;
char			textString[64];
char			obafkgmLetters[]	=	"OBAFGKM";
int				colourNum;

//	CONSOLE_DEBUG(__FUNCTION__);

	myCVrect.x		=	theWidget->left;
	myCVrect.y		=	theWidget->top;
	myCVrect.width	=	theWidget->width;
	myCVrect.height	=	theWidget->height;

	cOpenCV_Image	=	openCV_Image;

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

	myCVcenter.x	=	myCVrect.x + (myCVrect.height / 2);
	myCVcenter.y	=	myCVrect.y + (myCVrect.height / 2);
	radius1			=	(myCVrect.height - 25) / 2;
	iii				=	0;
	colourNum		=	W_STAR_O;

//	CONSOLE_DEBUG_W_NUM("height\t=", myCVrect.height);
//	CONSOLE_DEBUG_W_NUM("radius1\t=", radius1);

	while (obafkgmLetters[iii] > 0)
	{
//		cvCircle(	openCV_Image,
//					myCVcenter,
//					radius1,				//*	radius
//					theWidget->textColor,
//					1,						//	int thickness CV_DEFAULT(1),
//					8,						//	int line_type CV_DEFAULT(8),
//					0);						//	int shift CV_DEFAULT(0));

		SetColor(colourNum);
		FillEllipse(myCVcenter.x, myCVcenter.y, radius1, radius1);

		textString[0]	=	obafkgmLetters[iii];
		textString[1]	=	0;
		pt1				=	myCVcenter;
		pt1.x			-=	10;
		pt1.y			+=	7;
		cvPutText(	openCV_Image,
					textString,
					pt1,
					&gTextFont[kFont_Triplex_Large],
					theWidget->textColor
					);

		myCVcenter.x	+=	2 * radius1;
		myCVcenter.x	+=	10;

		colourNum++;
		iii++;
	}

}


//**************************************************************************************
void	WindowTabSTsettings::DrawGraphWidget(IplImage *openCV_Image, const int widgetIdx)
{
	switch(widgetIdx)
	{
		case kSkyT_Settings_OBAFGKM:
			DrawOBAFGKM(openCV_Image, &cWidgetList[widgetIdx]);
			break;

		default:
			CONSOLE_DEBUG_W_NUM("widgetIdx\t",	widgetIdx);
			break;
	}
}

static	pthread_t	gShellScript_ThreadID;
static	char		gShellCmdLine[64];
static	bool		gShellThreadIsRunning	=	false;

//*****************************************************************************
static void	*RunShellScript_Thead(void *arg)
{
int		systemRetCode;

	gShellThreadIsRunning	=	true;
	CONSOLE_DEBUG(__FUNCTION__);

	if (arg != NULL)
	{

		CONSOLE_DEBUG(arg);

		CONSOLE_DEBUG_W_STR("cmdLine\t=",	(char *)arg);
		systemRetCode	=	system((char *)arg);
		if (systemRetCode == 0)
		{
		}
		else
		{
			CONSOLE_DEBUG_W_NUM("ERROR system() returned", systemRetCode);
		}
	}
	else
	{
		CONSOLE_DEBUG("arg is NULL");
	}
	CONSOLE_DEBUG("Thread exiting");

	gShellThreadIsRunning	=	false;
	return(NULL);
}

//*****************************************************************************
void	RunShellScript(const char *commandLine)
{
int		threadStatus;
int		threadErr;

	CONSOLE_DEBUG(__FUNCTION__);

	if (gShellThreadIsRunning == false)
	{
		strcpy(gShellCmdLine, commandLine);
		CONSOLE_DEBUG_W_STR("gShellCmdLine\t=",	gShellCmdLine);

		threadErr	=	pthread_create(	&gShellScript_ThreadID,
										NULL,
										&RunShellScript_Thead,
										(void *)gShellCmdLine);
		if (threadErr == 0)
		{
			CONSOLE_DEBUG("Shell script thread created successfully");
			threadStatus	=	0;
		}
		else
		{
			CONSOLE_DEBUG_W_NUM("Error on thread creation, Error number:", threadErr);
		}
		CONSOLE_DEBUG("EXIT");
	}
	else
	{
		CONSOLE_DEBUG("Thread currently busy!!!!");
	}
}

//*****************************************************************************
void	WindowTabSTsettings::ProcessButtonClick(const int buttonIdx)
{
int		systemRetCode;

//	CONSOLE_DEBUG(__FUNCTION__);
//	CONSOLE_DEBUG_W_NUM("buttonIdx\t=", buttonIdx);

	switch(buttonIdx)
	{
		case kSkyT_Settings_EarthThin:
		case kSkyT_Settings_EarthThick:
		case kSkyT_Settings_EarthSolidBrn:
		case kSkyT_Settings_EarthSolidGrn:
			gST_DispOptions.EarthDispMode	=	buttonIdx - kSkyT_Settings_EarthThin;
			break;

		case kSkyT_Settings_DayNightSky:
			gST_DispOptions.DayNightSkyColor	=	!gST_DispOptions.DayNightSkyColor;
			break;

		case kSkyT_Settings_GridSolid:
			gST_DispOptions.DashedLines	=	false;
			break;

		case kSkyT_Settings_GridDashed:
			gST_DispOptions.DashedLines	=	true;
			break;

		case kSkyT_Settings_LineW_Const1:
			gLineWidth_Constellations	=	1;
			break;

		case kSkyT_Settings_LineW_Const2:
			gLineWidth_Constellations	=	2;
			break;

		case kSkyT_Settings_LineW_ConstOutlines1:
			gLineWidth_ConstOutlines	=	1;
			break;

		case kSkyT_Settings_LineW_ConstOutlines2:
			gLineWidth_ConstOutlines	=	2;
			break;

		case kSkyT_Settings_LineW_Grid1:
			gLineWidth_GridLines	=	1;
			break;

		case kSkyT_Settings_LineW_Grid2:
			gLineWidth_GridLines	=	2;
			break;

		case kSkyT_Settings_ResetToDefault:
			gST_DispOptions.EarthDispMode		=	0;
			gST_DispOptions.DashedLines			=	false;
			gST_DispOptions.DayNightSkyColor	=	false;
			gLineWidth_Constellations			=	1;
			gLineWidth_ConstOutlines			=	1;
			gLineWidth_GridLines				=	1;
			break;

		case kSkyT_Settings_DispMag:
			gST_DispOptions.DispMagnitude		=	!gST_DispOptions.DispMagnitude;
			break;

		case kSkyT_Settings_DispSpectral:
			gST_DispOptions.DispSpectralType	=	!gST_DispOptions.DispSpectralType;
			break;

		case kSkyT_Settings_DispDynMagnitude:
			gST_DispOptions.MagnitudeMode	=	kMagnitudeMode_Dynamic;
			break;

		case kSkyT_Settings_DispSpecifiedMagnitude:
			gST_DispOptions.MagnitudeMode	=	kMagnitudeMode_Specified;
			break;

		case kSkyT_Settings_DispAllMagnitude:
			gST_DispOptions.MagnitudeMode	=	kMagnitudeMode_All;
			break;

		case kSkyT_Settings_MagUpArrow:
			gST_DispOptions.DisplayedMagnitudeLimit++;
			if (gST_DispOptions.DisplayedMagnitudeLimit > 25)
			{
				gST_DispOptions.DisplayedMagnitudeLimit	=	25;
			}
			break;

		case kSkyT_Settings_MagDownArrow:
			gST_DispOptions.DisplayedMagnitudeLimit--;
			if (gST_DispOptions.DisplayedMagnitudeLimit < 0)
			{
				gST_DispOptions.DisplayedMagnitudeLimit	=	0;
			}
			break;

		case kSkyT_Settings_RunStartup:
			RunShellScript("./startup.sh");
			break;

		case kSkyT_Settings_RunShutdown:
			RunShellScript("./shutdown.sh");
			break;


	}
	UpdateSettings();
}

//*****************************************************************************
void	WindowTabSTsettings::ProcessDoubleClick(const int	widgetIdx,
												const int	event,
												const int	xxx,
												const int	yyy,
												const int	flags)
{

	switch(widgetIdx)
	{
		case kSkyT_Settings_OriginalData_txt:
		case kSkyT_Settings_OriginalData_cnt:
			if ((gStarDataPtr != NULL) && (gStarCount > 0))
			{
				CreateStarlistWindow("Original Database", gStarDataPtr, gStarCount);
			}
			break;

		case kSkyT_Settings_DataAAVSO_txt:
		case kSkyT_Settings_DataAAVSO_cnt:
		//	CreateAAVSOlistWindow();
			if ((gAAVSOalertsPtr != NULL) && (gAAVSOalertsCnt > 0))
			{
				CreateStarlistWindow("AAVSO List", gAAVSOalertsPtr, gAAVSOalertsCnt);
			}
			break;

		case kSkyT_Settings_DataMessier_txt:
		case kSkyT_Settings_DataMessier_cnt:
			if ((gMessierObjectPtr != NULL) && (gMessierObjectCount > 0))
			{
				CreateStarlistWindow("Messier List", gMessierObjectPtr, gMessierObjectCount, "M#");
			}
			break;

		case kSkyT_Settings_DataNGC_txt:
		case kSkyT_Settings_DataNGC_cnt:
			if ((gNGCobjectPtr != NULL) && (gNGCobjectCount > 0))
			{
				CreateStarlistWindow("NGC List", gNGCobjectPtr, gNGCobjectCount, "NGC/IC#");
			}
			break;


		case kSkyT_Settings_DataYALE_txt:
		case kSkyT_Settings_DataYALE_cnt:
			if ((gYaleStarDataPtr != NULL) && (gYaleStarCount > 0))
			{
				CreateStarlistWindow("Yale Star Catalog", gYaleStarDataPtr, gYaleStarCount);
			}
			break;

		case kSkyT_Settings_Constellations_txt:
		case kSkyT_Settings_Constellations_cnt:
			if ((gConstVecotrPtr != NULL) && (gConstVectorCnt > 0))
			{
				CreateConstellationListWindow(gConstVecotrPtr, gConstVectorCnt);
			}
			break;

	#ifdef _ENABLE_REMOTE_GAIA_
		case kSkyT_Settings_Gaia_txt:
		case kSkyT_Settings_Gaia_cnt:
			CreateGaiaRemoteListWindow();
			break;
	#endif // _ENABLE_REMOTE_GAIA_

	}
}


//*****************************************************************************
//*	update the various settings check boxes etc
//*****************************************************************************
void	WindowTabSTsettings::UpdateSettings(void)
{
char	textString[32];

//	CONSOLE_DEBUG(__FUNCTION__);
	SetWidgetChecked(	kSkyT_Settings_EarthThin,		(gST_DispOptions.EarthDispMode == 0));
	SetWidgetChecked(	kSkyT_Settings_EarthThick,		(gST_DispOptions.EarthDispMode == 1));
	SetWidgetChecked(	kSkyT_Settings_EarthSolidBrn,	(gST_DispOptions.EarthDispMode == 2));
	SetWidgetChecked(	kSkyT_Settings_EarthSolidGrn,	(gST_DispOptions.EarthDispMode == 3));

	SetWidgetChecked(	kSkyT_Settings_DayNightSky,		gST_DispOptions.DayNightSkyColor);

	SetWidgetChecked(	kSkyT_Settings_GridSolid,				(!gST_DispOptions.DashedLines));
	SetWidgetChecked(	kSkyT_Settings_GridDashed,				(gST_DispOptions.DashedLines));

	SetWidgetChecked(	kSkyT_Settings_LineW_Const1,			(gLineWidth_Constellations == 1));
	SetWidgetChecked(	kSkyT_Settings_LineW_Const2,			(gLineWidth_Constellations == 2));

	SetWidgetChecked(	kSkyT_Settings_LineW_ConstOutlines1,	(gLineWidth_ConstOutlines == 1));
	SetWidgetChecked(	kSkyT_Settings_LineW_ConstOutlines2,	(gLineWidth_ConstOutlines == 2));

	SetWidgetChecked(	kSkyT_Settings_LineW_Grid1,				(gLineWidth_GridLines == 1));
	SetWidgetChecked(	kSkyT_Settings_LineW_Grid2,				(gLineWidth_GridLines == 2));

	SetWidgetChecked(	kSkyT_Settings_DispMag,				gST_DispOptions.DispMagnitude);
	SetWidgetChecked(	kSkyT_Settings_DispSpectral,		gST_DispOptions.DispSpectralType);


	SetWidgetChecked(	kSkyT_Settings_DispDynMagnitude,
				(gST_DispOptions.MagnitudeMode == kMagnitudeMode_Dynamic));

	SetWidgetChecked(	kSkyT_Settings_DispSpecifiedMagnitude,
				(gST_DispOptions.MagnitudeMode == kMagnitudeMode_Specified));

	SetWidgetChecked(	kSkyT_Settings_DispAllMagnitude,
				(gST_DispOptions.MagnitudeMode == kMagnitudeMode_All));

	sprintf(textString, "%1.1f", gST_DispOptions.DisplayedMagnitudeLimit);
	SetWidgetText(kSkyT_Settings_MagnitudeLimit, textString);

	ForceUpdate();
}



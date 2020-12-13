//*****************************************************************************
//*	Name:			RequestData.h
//*
//*	Author:			Mark Sproul
//*
//*	Description:
//*
//*	Limitations:
//*
//*	Usage notes:
//*
//*****************************************************************************
//*	Edit History
//*****************************************************************************
//*	<MLS>	=	Mark L Sproul
//*****************************************************************************

//#include	"RequestData.h"

#ifndef _REQUESTDATA_H_
#define	_REQUESTDATA_H_

#ifndef	_ALPACA_DEFS_H_
	#include	"alpaca_defs.h"
#endif // _ALPACA_DEFS_H_

//*****************************************************************************
//*	the TYPE_GetPutRequestData simplifies parsing and passing of the
//*	parsed data to subroutines
#define	kDeviceTypeMaxLen	64
#define	kDevStrLen			2048
#define	kMaxCommandLen		64
#define	kHTMLbufLen			4096

#define	kMaxJsonHdrLen		512
#define	kMaxJsonBuffLen		(10 * 1024)
//*****************************************************************************
typedef struct
{
	int					socket;
	int					deviceNumber;
	char				get_putIndicator;
	int					contentLength;
	char				htmlData[kHTMLbufLen];
	char				deviceType[kDeviceTypeMaxLen];
	char				cmdBuffer[kDevStrLen];
	char				deviceCommand[kMaxCommandLen];
	char				contentData[kDevStrLen];
	TYPE_ASCOM_STATUS	alpacaErrCode;
	char				alpacaErrMsg[256];
	//*	outgoing data
	char				jsonHdrBuffer[kMaxJsonHdrLen];
	char				jsonTextBuffer[kMaxJsonBuffLen];
} TYPE_GetPutRequestData;

void	DumpRequestStructure(const char *functionName, TYPE_GetPutRequestData	*reqData);
//--int		Common_ProcessCommand(TYPE_GetPutRequestData *reqData, int cmdEnum);


#endif	//	_REQUESTDATA_H_

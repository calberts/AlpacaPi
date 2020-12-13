//**************************************************************************
//*	Name:			JsonResponse.h
//*
//*	Author:			Mark Sproul
//*
//*****************************************************************************
//#include	"JsonResponse.h"

#ifndef _JSON_RESPONSE_H_
#define	_JSON_RESPONSE_H_

#ifndef _STDBOOL_H
	#include	<stdbool.h>
#endif

#ifndef _STDINT_H
	#include	<stdint.h>
#endif

#ifdef __cplusplus
	extern "C" {
#endif


//int		JsonResponse_SendTextBuffer(int socketFD, const char *jsonTextBuffer);

//*	routines that write to a text buffer and xmit if needed
void	JsonResponse_CreateHeader(char *jsonTextBuffer, const int maxLen);



void	JsonResponse_FinishHeader(	char *jsonHdrBUffer, const char *jsonTextBuffer);


void	JsonResponse_Add_HDR(		char *jsonTextBuffer, const int maxLen);
void	JsonResponse_Add_Data(		const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen);
void	JsonResponse_Add_String(	const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen,
									const char		*itemName,
									const char		*stringValue,
									bool			includeTrailingComma);

void	JsonResponse_Add_Int32(		const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen,
									const char		*itemName,
									const int32_t	intValue,
									bool			includeTrailingComma);

void	JsonResponse_Add_Double(	const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen,
									const char		*itemName,
									const double	dblValue,
									bool			includeTrailingComma);

void	JsonResponse_Add_Bool(		const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen,
									const char		*itemName,
									const bool		boolValue,
									bool			includeTrailingComma);

void	JsonResponse_Add_EndBlock(	const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen,
									bool			includeTrailingComma);

void	JsonResponse_Add_ArrayStart(const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen,
									const char		*itemName);

void	JsonResponse_Add_ArrayEnd(	const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen,
									bool			includeTrailingComma);

#define	kInclude_HTTP_Header	true
#define	kNo_HTTP_Header			false
void	JsonResponse_Add_Finish(	const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen,
									bool			includeHeader);

void	JsonResponse_Add_RawText(	const int		socketFD,
									char			*jsonTextBuffer,
									const int		maxLen,
									const char		*rawTextBuffer);
int		JsonResponse_SendTextBuffer(int socketFD, const char *jsonTextBuffer);

#define	INCLUDE_COMMA	true
#define	NO_COMMA		false


#ifdef __cplusplus
}
#endif

#endif	//	_JSON_RESPONSE_H_

/*! \file
***************************************************************************************************************
file name:					stringhelper.h
*	\copyright				FourE
*	\brief					stringhelper header file
*	\author					Frank Eggink
*	\date	created:			2020-03-02

***************************************************************************************************************
\par	Description
***************************************************************************************************************

	Description

***************************************************************************************************************
\note
***************************************************************************************************************

	note description

***************************************************************************************************************
*/

#ifndef __STRINGHELPER_H_
#define __STRINGHELPER_H_

/*
***************************************************************************************************************
	INCLUDE FILES
***************************************************************************************************************
*/

#include <stdint.h>

/*
***************************************************************************************************************
	Make header CPP compatible
***************************************************************************************************************
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
***************************************************************************************************************
	GLOBAL DEFINES
***************************************************************************************************************
*/


/*
***************************************************************************************************************
	GLOBAL TYPEDEF
***************************************************************************************************************
*/

typedef enum {
	 WT_LEADING = 0
	,WT_TRAILING
	,WT_ALL
} E_whitespaceType;

/*
***************************************************************************************************************
	GLOBAL VARS
***************************************************************************************************************
*/


/*
***************************************************************************************************************
	GLOBAL FUNCTION PROTOTYPES
***************************************************************************************************************
*/

void RemoveWhitespace(const char* input, char* output, E_whitespaceType type);
void RemoveComments(const char* input, char* output);
void TrimString(const char* input, char* output);
void RemoveCommentsAndTrim(const char* input, char* output);
uint8_t IsLineComment(const char* input);
void StripExtension(const char* input, char* output);
void ExtractFileName(const char* input, char* output);
uint8_t GetDirectoryNameAndLength(const char* input, char* output);
uint8_t HasFileNameExtension(const char* input, const char* extension);

/*
***************************************************************************************************************

***************************************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif		// __STRINGHELPER_H_

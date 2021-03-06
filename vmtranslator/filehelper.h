/*! \file
***************************************************************************************************************
file name:					filehelper.h
*	\copyright				FourE
*	\brief					filehelper header file
*	\author					Frank Eggink
*	\date	created:			2020-03-21

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

#ifndef __FILEHELPER_H
#define __FILEHELPER_H

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
	 IFT_NONE = 0
	,IFT_SINGLE_VM_FILE
	,IFT_DIRECTORY
} E_inputFileType;

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

E_inputFileType GetInputFileType(char* input);
void CreateOutputFileName(const char* input, char* output, E_inputFileType inputFileType);
int8_t GetNumberOfFilesInDirectory(char* directoryName, char* extension);

/*
***************************************************************************************************************

***************************************************************************************************************
*/

#ifdef __cplusplus
}
#endif

#endif		// __FILEHELPER_H

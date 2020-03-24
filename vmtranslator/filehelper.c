/*! \file
***************************************************************************************************************
file name:					filehelper.c
*	\copyright				FourE
*	\brief					filehelper source file
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


/*
***************************************************************************************************************
	INCLUDE FILES
***************************************************************************************************************
*/

#include "filehelper.h"
#include <stdio.h>
#include <string.h>				// strlen
#include <dirent.h>
#include "stringhelper.h"

/*
***************************************************************************************************************
	LOCAL DEFINES
***************************************************************************************************************
*/

#define MAX_DIR_NAME_LENGTH	(50)

/*
***************************************************************************************************************
	LOCAL TYPEDEFS
***************************************************************************************************************
*/


/*
***************************************************************************************************************
	LOCAL FUNCTION PROTOTYPES
***************************************************************************************************************
*/


/*
***************************************************************************************************************
	GLOBAL VARS
***************************************************************************************************************
*/


/*
***************************************************************************************************************
	LOCAL VARS
***************************************************************************************************************
*/


/*
***************************************************************************************************************
	IMPLEMENTATION
***************************************************************************************************************
*/

E_inputFileType GetInputFileType(char* input) {
/*!
***************************************************************************************************************

	\description
		Function determines if input string is a .vm file or a directory that holds .vm files

	\param[in]		input		Pointer to input string

	\returns
			IFT_SINGLE_VM_FILE	: input is a single .vm file
			IFT_DIRECTORY			: input is a directory with .vm files inside
			IFT_NONE					: no .vm files were found

	\note
		- make sure that input is not NULL
		- make sure input string is nul terminated with '\0

***************************************************************************************************************
*/
	E_inputFileType result = IFT_NONE;

	// check if input is a single .vm file
	if (HasFileNameExtension(input, ".vm") != 0) {
		result = IFT_SINGLE_VM_FILE;
	} else {
		if (GetNumberOfFilesInDirectory(input, ".vm") > 0) {
			result = IFT_DIRECTORY;
		}
	}
	return result;
}
/*
***************************************************************************************************************
	End GetInputFileType
***************************************************************************************************************
*/

void CreateOutputFileName(const char* input, char* output, E_inputFileType inputFileType) {
/*!
***************************************************************************************************************

	\description
		Function takes the input string (either fileName of directory name) and creates an output fileName (.asm)

	\param[in]		input				Pointer to input string
	\param[out]		output			Pointer to output string
	\param[in]		inputFileType	File type of the input

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	char directoryName[MAX_DIR_NAME_LENGTH] = { 0 };

	memset(directoryName, 0, sizeof(directoryName));

	switch (inputFileType) {
	case IFT_SINGLE_VM_FILE:
		// get rid of extension
		StripExtension(input, output);
		// add new extension
		strcat(output, ".asm");
		break;
	case IFT_DIRECTORY:
		// DIRECTORY
		GetDirectoryNameAndLength(input, directoryName);
		strcpy(output, input);
		strcat(output, "/");  // this seems to work under windows (otherwise we would have added "\\")
		strcat(output, directoryName);
		strcat(output, ".asm");
		break;
	default:
		break;
	}
}
/*
***************************************************************************************************************
	End CreateOutputFileName
***************************************************************************************************************
*/

int8_t GetNumberOfFilesInDirectory(char* directoryName, char* extension) {
/*!
***************************************************************************************************************

	\description
		Function removes the trailing whitespace from a string

	\param[in]		input			Pointer to directory name
	\param[in]		extension	Pointer to extension

	\returns
			0		: NO files were found with the specified extension in the directory
			> 0	: the number or files found in the directory with the specified extension

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0

***************************************************************************************************************
*/
	struct dirent *pDirent;
	DIR *pDir;
	uint8_t count = 0;

	pDir = opendir(directoryName);
	if (pDir == NULL) {
		printf ("Cannot open directory '%s'\n", directoryName);
		return -1;
	}

	pDirent = readdir(pDir);

	while (pDirent != NULL) {
	  if (HasFileNameExtension(pDirent->d_name, extension) != 0) {
			printf ("[%s]\n", pDirent->d_name);
			count++;
	  }
	  pDirent = readdir(pDir);
	}

	closedir (pDir);
	return count;
}
/*
***************************************************************************************************************
	End GetNumberOfFilesInDirectory
***************************************************************************************************************
*/

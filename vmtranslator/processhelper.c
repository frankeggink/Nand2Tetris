/*! \file
***************************************************************************************************************
file name:					processhelper.c
*	\copyright				FourE
*	\brief					process helper source file
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

#include "processhelper.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include "stringhelper.h"
#include "parser.h"
#include "codewriter_hack.h"

/*
***************************************************************************************************************
	LOCAL DEFINES
***************************************************************************************************************
*/

#define MAX_LINE_LENGTH			(256)

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

// TODO maybe merge Process File and OutputCode ??
void OutputCode(FILE* inputFile, FILE* outputFile, char* fileName) {
/*!
***************************************************************************************************************

	\description
		Function processes each line in the inputFile and calls the codewriter to assemble each VM command

	\param[in]		inputFile		Pointer to input file
	\param[out]		outputFile		Pointer to output file
	\param[in]		filename			Pointer to fileName

	\note
		- make sure that both inputFile and outputFile are opened
		- make sure fileName string is nul terminated with '\0
		- fileName is used to generate file specific labels for the assembly code output

***************************************************************************************************************
*/
	char lineBuffer[MAX_LINE_LENGTH];
	char parseBuffer[MAX_LINE_LENGTH];
	uint32_t lineNumber = 0; // used to indicate where an error is detected in the input file

	while (fgets(lineBuffer, MAX_LINE_LENGTH, inputFile) != NULL) {
		lineNumber++;
		// skip commments
		if (IsLineComment(lineBuffer) == 0) {
			RemoveCommentsAndTrim(lineBuffer, lineBuffer);
			if (lineBuffer[0] != '\0') { // handles special case (skip blank lines)
				// make a copy to be able to output the VM command as a comment in the output file for debugging purposes
				memcpy(parseBuffer, lineBuffer, MAX_LINE_LENGTH);
				// we need to make copy because we want to have original line of VM code as a comment in the output file
				// parseCommand will change the string because it tokenizes it!!
				if (ParseCommand(parseBuffer) != 0) {
					E_commandType command;
					command = GetCommandType();
					// TODO refactor WriteCommand
					// TODO Get parser data (struct) and pass that as parameter to WriteCommand
					// string inside lineBuffer is output to output file as a comment for debug purposes
					WriteCommand(outputFile, lineBuffer, fileName, command);
				} else {
					printf("Error in file: %s on source line #%d\n", fileName, lineNumber);
				}
			}
		}
	}
}
/*
***************************************************************************************************************
	End OutputCode
***************************************************************************************************************
*/

uint8_t ProcessDirectory(const char* directory, FILE* outputFile) {
/*!
***************************************************************************************************************

	\description
		Function processes each .vm file that it finds in the specified directory

	\param[in]		directory		Pointer to path string
	\param[out]		outputFile		Pointer to output file

	\returns
			0: directory could not be opened
			1: processing directory was successful

	\note
		- make sure that directory is not NULL
		- make sure directory string is nul terminated with '\0
		- make sure that outputFile is opened

***************************************************************************************************************
*/
	char inputFileName[MAX_FILENAME_LENGTH] = { 0 };

	WriteInit(outputFile);

	struct dirent *pDirent;
	DIR *pDir;

	pDir = opendir(directory);
	if (pDir == NULL) {
		printf ("Cannot open directory '%s'\n", directory);
		return 0;
	}

	pDirent = readdir(pDir);

	while (pDirent != NULL) {
		if (HasFileNameExtension(pDirent->d_name, ".vm") != 0) {
			//printf ("[%s]\n", pDirent->d_name);
			memset(inputFileName, 2, sizeof(inputFileName));
			strcpy(inputFileName, directory);
			strcat(inputFileName, "/"); // this seems to work under windows (otherwise we would have added "\\")
			strcat(inputFileName, pDirent->d_name);
			ProcessVMFile(inputFileName, outputFile);
	  }
	  pDirent = readdir(pDir);
	}
	closedir (pDir);

	return 1;
}
/*
***************************************************************************************************************
	End ProcessDirectory
***************************************************************************************************************
*/

uint8_t ProcessVMFile(char* inputFileName, FILE* outputFile) {
/*!
***************************************************************************************************************

	\description
		Function processes input .vm file

	\param[in]		inputFileName		Pointer to input fileName
	\param[out]		outputFile			Point to output file

	\returns
			0: processing of .vm file was NOT successful
			1: processing of .vm file successful

	\note
		- make sure that inputFileName is not NULL
		- make sure inputFileName string is nul terminated with '\0
		- make sure that outputFile is opened

***************************************************************************************************************
*/
	FILE* pFile = NULL;

	// try to open input file
	pFile = fopen(inputFileName, "r");
	if (pFile == NULL) {
		printf( "Could not open input file\n" );
		return 0;
	}

	// write filename of input file as a comment to the output file
	fprintf(outputFile, "//input file: %s\n", inputFileName);

	// also create a string that is used to generate file specific labels in the assembly code
	// https://stackoverflow.com/questions/7180293/how-to-extract-filename-from-path
	ExtractFileName(inputFileName, inputFileName);
	StripExtension(inputFileName, inputFileName);

	// write assembly code to the output file
	OutputCode(pFile, outputFile, inputFileName);

	// cleanup stuff
	if (pFile != NULL) {
		fclose(pFile);
		pFile = NULL;
	}
	return 1;
}
/*
***************************************************************************************************************
	End ProcessVMFile
***************************************************************************************************************
*/

/*! \file
***************************************************************************************************************
file name:					stringhelper.c
*	\copyright				FourE
*	\brief					stringhelper source file
*	\author					Frank Eggink
*	\date	created:			2020-02-03

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

#include "stringhelper.h"
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <ctype.h>	// isspace
#include <string.h>	// strlen

/*
***************************************************************************************************************
	LOCAL DEFINES
***************************************************************************************************************
*/


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

static void RemoveLeadingWhitespace(const char* input, char* output);
static void RemoveTrailingWhitespace(const char* input, char* output);
static void RemoveAllWhitespace(const char* input, char* output);

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

static void RemoveLeadingWhitespace(const char* input, char* output) {
/*!
***************************************************************************************************************

	\description
		Function removes the leading whitespace from a string

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	uint8_t in = 0;
	uint8_t out = 0;
	uint8_t leadingSkipped = 0;

	while (input[in] != '\0') {

		// skip leading whitespace
		while (	(isspace(input[in]) != 0)
				&& (leadingSkipped == 0)
		) {
			in++;
		}

		leadingSkipped = 1;

		// now copy rest of string
		output[out] = input[in];
		out++;
		in++;
	}

	// terminate string
	output[out] = '\0';
}
/*
***************************************************************************************************************
	End RemoveLeadingWhitespace
***************************************************************************************************************
*/

static void RemoveTrailingWhitespace(const char* input, char* output) {
/*!
***************************************************************************************************************

	\description
		Function removes the trailing whitespace from a string

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	uint8_t in = 0;
	uint8_t lastIndex = 0;

	while (input[in] != '\0') {
		if (isspace(input[in]) == 0) {
			lastIndex = in;
		}
		in++;
	}

	if (lastIndex > 0) {
		lastIndex += 1;
		// copy string
		memcpy(output, input, sizeof(char) * lastIndex);
	}
	// terminate string
	output[lastIndex] = '\0';
}
/*
***************************************************************************************************************
	End RemoveTrailingWhitespace
***************************************************************************************************************
*/

static void RemoveAllWhitespace(const char* input, char* output) {
/*!
***************************************************************************************************************

	\description
		Function removes all whitespace from a string

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	uint8_t in = 0;
	uint8_t out = 0;

	while(input[in] != '\0') {
		// check if it is not whitespace
		if (isspace(input[in]) == 0) {
			output[out] = input[in];
			out++;
		}
		in++;
	}

	// terminate string
	output[out] = '\0';
}
/*
***************************************************************************************************************
	End RemoveAllWhitespace
***************************************************************************************************************
*/

void RemoveWhitespace(const char* input, char* output, E_whitespaceType type) {
/*!
***************************************************************************************************************

	\description
		Function removes whitespace from a string based on the selected type

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string
	\param[in]		type		determines the behavior of the function

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	switch (type) {
	case WT_LEADING:
		RemoveLeadingWhitespace(input, output);
		break;
	case WT_TRAILING:
		RemoveTrailingWhitespace(input, output);
		break;
	case WT_ALL:
		RemoveAllWhitespace(input, output);
		break;
	default:
		break;
	}
}
/*
***************************************************************************************************************
	End RemoveWhitespace
***************************************************************************************************************
*/

void TrimString(const char* input, char* output) {
/*!
***************************************************************************************************************

	\description
		Function removes the leading and trailing whitespace from a string

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	RemoveLeadingWhitespace(input, output);

	// use output from previous function as input for next function
	RemoveTrailingWhitespace(output, output);
}
/*
***************************************************************************************************************
	End TrimString
***************************************************************************************************************
*/

void RemoveComments(const char* input, char* output) {
/*!
***************************************************************************************************************

	\description
		Function removes C99 style comments from a string
		C99 style comments look like:
		// this is a comment

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	uint8_t in = 0;
	uint8_t out = 0;
	uint8_t commentFound = 0;

	// CHECK THIS
	while (	(input[in] != '\0')
			&& (commentFound == 0)
	) {

		if (	(input[in] == '/')
			&& (input[in + 1] == '/')
		) {
			commentFound = 1;
		} else {
			output[out] = input[in];
			out++;
			in++;
		}
	}

	// terminate string
	output[out] = '\0';
}
/*
***************************************************************************************************************
	End RemoveComments
***************************************************************************************************************
*/

void RemoveCommentsAndTrim(const char* input, char* output) {
/*!
***************************************************************************************************************

	\description
		Function removes the leading and trailing whitespace from a string
		Function also removes C99 style comments from a string
		C99 style comments look like:
		// this is a comment

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	RemoveComments(input, output);

	// use output from previous function as input for next function
	TrimString(output, output);
}
/*
***************************************************************************************************************
	End RemoveCommentsAndTrim
***************************************************************************************************************
*/

uint8_t IsLineComment(const char* input) {
/*!
***************************************************************************************************************

	\description
		Function checks if a string (line) is a C99-style commment
		C99 style comments look like:
		// this is a comment

		Any leading whitespace on the line is ignored by the function

	\param[in]		input		Pointer to input string

	\returns
			0: line is NOT comment
			1: line is a comment

	\note
		- make sure that input is not NULL
		- make sure input string is nul terminated with '\0

***************************************************************************************************************
*/
	assert(input != NULL);

	uint8_t in = 0;

	while(input[in] != '\0') {
		while (isspace(input[in]) != 0) {
			in++;
		}

		if (	(input[in] == '/')
			&& (input[in + 1] == '/')
		) {
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
}
/*
***************************************************************************************************************
	End IsLineComment
***************************************************************************************************************
*/

void StripExtension(const char* input, char* output) {
/*!
***************************************************************************************************************

	\description
		Function removes the extension from a string
		Examples:
		dummyFile.asm (.asm is removed)
		fakeFile.c (.c is removed)

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	uint8_t in = 0;
	uint8_t lastIndex = 0;

	while (input[in] != '\0') {
		if (input[in] == '.') {
			lastIndex = in;
		}
		in++;
	}

	if (lastIndex > 0) {
		// lastIndex is location of '.' char
		// copy string
		memcpy(output, input, sizeof(char) * lastIndex);

		// terminate string
		output[lastIndex] = '\0';
	}

//	// terminate string
//	output[lastIndex] = '\0';
}
/*
***************************************************************************************************************
	End StripExtension
***************************************************************************************************************
*/

void ExtractFileName(const char* input, char* output) {
/*!
***************************************************************************************************************

	\description
		Function extracts the filename from a input string

		Examples:
		c:\\foo\\bar\\vmTest.asm\\
		vmTest.asm
		c:\\foo\\bar\\vmTest.asm
		vmTest.asm\\
		c:/foo/bar/vmTest.asm/
		c:/foo/bar/vmTest.asm

		The output in all cases is: vmTest.asm

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	uint8_t lastIndex;
	uint8_t firstIndex;

	lastIndex = strlen(input);

	lastIndex = lastIndex - 1;


	if (	(input[lastIndex] == '\\')
		|| (input[lastIndex] == '/')
	) {
		lastIndex = lastIndex - 1;
	}

	firstIndex = lastIndex;
	while (	(input[firstIndex] != '\\')
			&& (input[firstIndex] != '/')
			&& (firstIndex > 0)
	) {
		firstIndex--;
	}

	if (firstIndex > 0) {
		firstIndex++;
	}
	memcpy(output, &input[firstIndex], sizeof(char) * ((lastIndex - firstIndex) + 1));

	// terminate string
	output[(lastIndex - firstIndex) + 1] = '\0';
}
/*
***************************************************************************************************************
	End ExtractFileName
***************************************************************************************************************
*/

uint8_t GetDirectoryNameAndLength(const char* input, char* output) {
/*!
***************************************************************************************************************

	\description
		Function extracts the directory from a input string and returns the length of the directory name

		Example:
		c:\foo\bar\baz

		The output is: baz (directory name)
		And the string length is 3

	\param[in]		input		Pointer to input string
	\param[out]		output	Pointer to output string

	\returns
		string length of the directory name

	\note
		- make sure that both input and output are not NULL
		- make sure input string is nul terminated with '\0
		- make sure that output has atleast the same size as input

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(output != NULL);

	uint8_t length = 0;

	ExtractFileName(input, output);

	length = strlen(output);

	return length;
}
/*
***************************************************************************************************************
	End GetDirectoryNameAndLength
***************************************************************************************************************
*/

uint8_t HasFileNameExtension(const char* input, const char* extension) {
/*!
***************************************************************************************************************

	\description
		Function checks if the input string (a filename) has got a certain extension

	\param[in]		input			Pointer to input string
	\param[in]		extension	Pointer to extension string

	\returns
			0: input string does NOT have the extension as specified
			1: input string has got the extension as specified

	\note
		- make sure that both input and extension are not NULL
		- make sure that both input and extension are nul terminated with '\0

***************************************************************************************************************
*/
	assert(input != NULL);
	assert(extension != NULL);

	uint8_t in = 0;
	uint8_t lastIndex = 0;

	while (input[in] != '\0') {
		if (input[in] == '.') {
			lastIndex = in;
		}
		in++;
	}

	if (lastIndex > 0) {
		// lastIndex is location of '.' char
		if (strcmp(&input[lastIndex], extension) == 0) {
			return 1;
		} else {
			return 0;
		}
	}
	return 0;
}
/*
***************************************************************************************************************
	End HasFileNameExtension
***************************************************************************************************************
*/

/*
***************************************************************************************************************
	TEST CODE
***************************************************************************************************************
*/

//void TestStringHelper(const char* input) {
//	char outString[100];
//
//	printf("LEADING: %s\n", input);
//	printf("------------------------------------------------------------------------------------------------------------------------------\n");
//	printf("%s\n", input);
//	RemoveWhitespace(input, outString, WT_LEADING);
//	printf("%s\n", outString);
//	printf("%s\n", input);
//	printf("\n");
//
//	printf("TRAILING: %s\n", input);
//	printf("------------------------------------------------------------------------------------------------------------------------------\n");
//	printf("%s\n", input);
//	RemoveWhitespace(input, outString, WT_TRAILING);
//	printf("%s\n", outString);
//	printf("%s\n", input);
//	printf("\n");
//
//	printf("ALL: %s\n", input);
//	printf("------------------------------------------------------------------------------------------------------------------------------\n");
//	printf("%s\n", input);
//	RemoveWhitespace(input, outString, WT_ALL);
//	printf("%s\n", outString);
//	printf("%s\n", input);
//	printf("\n");
//
//}

//int main(int argc, char* argv[]) {
//
//	char* emptyString = "";
//	char* string1space = " ";
//	char* string2space = "	";
//	char* string1tab = "	";
//	char* testString = "        						 dit     is    een        teststring         			   ";
//	char* commentEnd = "  		  this line contains a comment at the end, 20 / 5 = 4... 		// comment				";
//	char* commentStart = "     			// comment  this is a comment string, 20 / 5 = 4... 		// comment		";
//	char out[100];
//
//	char* filename = "ditiseentestbestand.extension";
//
//	TestStringHelper(emptyString);
//	TestStringHelper(string1space);
//	TestStringHelper(string2space);
//	TestStringHelper(string1tab);
//	TestStringHelper(testString);
//	TestStringHelper(commentEnd);
//	TestStringHelper(commentStart);
//
//
//	StripExtension(filename, out);
//	printf("%s\n", filename);
//	printf("%s\n", out);
//}

//#include <stdio.h>
//int main(int argc, char* argv[]) {
//
//	char* string = "c:\\frank\\eggink\\vmTest.asm\\";
//	char* string2 = "vmTest.asm";
//	char* string3 = "c:\\frank\\eggink\\vmTest.asm";
//	char* string4 = "vmTest.asm\\";
//	char* string5 = "c:/frank/eggink/vmTest.asm/";
//	char* string6 = "c:/frank/eggink/vmTest.asm";
//	char* string7 = "vmTest.asm/";
//	char* stringNull = NULL;
//
//
//	char out[100];
//
//
//
//	ExtractFileName(string, out);
//	printf("%s\n", string);
//	printf("%s\n", out);
//	printf("\n");
//
//	memset(out, 0, sizeof(out));
//	ExtractFileName(string2, out);
//	printf("%s\n", string2);
//	printf("%s\n", out);
//	printf("\n");
//
//	memset(out, 0, sizeof(out));
//	ExtractFileName(string3, out);
//	printf("%s\n", string3);
//	printf("%s\n", out);
//	printf("\n");
//
//	memset(out, 0, sizeof(out));
//	ExtractFileName(string4, out);
//	printf("%s\n", string4);
//	printf("%s\n", out);
//	printf("\n");
//
//	memset(out, 0, sizeof(out));
//	ExtractFileName(string5, out);
//	printf("%s\n", string5);
//	printf("%s\n", out);
//	printf("\n");
//
//	memset(out, 0, sizeof(out));
//	ExtractFileName(string6, out);
//	printf("%s\n", string6);
//	printf("%s\n", out);
//	printf("\n");
//
//	memset(out, 0, sizeof(out));
//	ExtractFileName(string7, out);
//	printf("%s\n", string7);
//	printf("%s\n", out);
//	printf("\n");
//
//	// TEST assert
//	ExtractFileName(stringNull, out);
//}
//


//#include <stdio.h>
//int main(int argc, char* argv[]) {
//
//	char* string = "c:\\frank\\eggink\\vmTest.asm\\";
//	char* string1 = "vmTest.asm";
//	char* string2 = "vmTest";
//	char* string3 = "c:/frank/eggink/vmTest.vm/";
//	char* string4 = "c:/frank/eggink/vmTest.vm";
//	char* string5 = "vmTest.vm/";
//	char* string6 = "vmTest";
//
//	printf("%s\n", string);
//	if (HasFileNameExtension(string, ".asm") != 0) {
//		printf("has extension\n");
//	} else {
//		printf("has NO extension\n");
//	}
//	printf("\n");
//
//	printf("%s\n", string1);
//	if (HasFileNameExtension(string1, ".asm") != 0) {
//		printf("has extension\n");
//	} else {
//		printf("has NO extension\n");
//	}
//	printf("\n");
//
//	printf("%s\n", string2);
//	if (HasFileNameExtension(string2, ".asm") != 0) {
//		printf("has extension\n");
//	} else {
//		printf("has NO extension\n");
//	}
//	printf("\n");
//
//	printf("%s\n", string3);
//	if (HasFileNameExtension(string3, ".asm") != 0) {
//		printf("has extension\n");
//	} else {
//		printf("has NO extension\n");
//	}
//	printf("\n");
//
//	printf("%s\n", string4);
//	if (HasFileNameExtension(string4, ".vm") != 0) {
//		printf("has extension\n");
//	} else {
//		printf("has NO extension\n");
//	};
//	printf("\n");
//
//	printf("%s\n", string5);
//	if (HasFileNameExtension(string5, ".vm") != 0) {
//		printf("has extension\n");
//	} else {
//		printf("has NO extension\n");
//	}
//	printf("\n");
//
//	printf("%s\n", string6);
//	if (HasFileNameExtension(string6, ".vm") != 0) {
//		printf("has extension\n");
//	} else {
//		printf("has NO extension\n");
//	}
//	printf("\n");
//}

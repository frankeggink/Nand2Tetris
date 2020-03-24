/*! \file
***************************************************************************************************************
file name:					codewriter_hack.c
*	\copyright				FourE
*	\brief					codewriter for hack source file
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

#include "codewriter_hack.h"
#include <stdio.h>
#include <string.h> // memset

/*
***************************************************************************************************************
	LOCAL DEFINES
***************************************************************************************************************
*/

#define POP_D		"//POP_D\n" \
						"@SP\nM=M-1\nD=M\nA=D\nD=M\n"

#define PUSH_D		"//PUSH_D\n" \
						"@SP\nA=M\nM=D\n@SP\nM=M+1\n"


// buffer size is increased from 512 to 1024
// WriteFunction outputs ~25 chars for each local variable a function uses
#define MAX_OUTPUT_LENGTH		(1024)


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

static void ClearOutputBuffer(void);
static uint8_t WriteArithmetic(E_commandType command, char* output);
static uint8_t WritePush(E_memorySegment memorySegment, uint16_t index, char* fileName, char* output);
static uint8_t WritePop(E_memorySegment memorySegment, uint16_t index, char* fileName, char* output);
static uint8_t WriteLabel(char* labelName,  char* fileName, char* output);
static uint8_t WriteGoto(char* labelName,  char* fileName, char* output);
static uint8_t WriteIfGoto(char* labelName,  char* fileName, char* output);
static uint8_t WriteFunction(char* labelName, uint8_t numLocals, char* output);
static uint8_t WriteCall(char* labelName, uint8_t numParams, char* output);
static uint8_t WriteReturn(char* output);

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

static char outputBuffer[MAX_OUTPUT_LENGTH];

/*
***************************************************************************************************************
	IMPLEMENTATION
***************************************************************************************************************
*/

static void ClearOutputBuffer(void) {
/*!
***************************************************************************************************************

	\description
		Function clears the output buffer

***************************************************************************************************************
*/
	memset(outputBuffer, 0, MAX_OUTPUT_LENGTH);
}
/*
***************************************************************************************************************
	End ClearOutputBuffer
***************************************************************************************************************
*/

// Or move function to main ? NOPE (all codewriting here !!!)
uint8_t WriteInit(FILE* pFile) {
/*!
***************************************************************************************************************

	\description
		Function generates assembly code for the Bootstrap code and writes it to a output buffer

	\param[out]		pFile			Pointer to output file

	\returns
		0: writing assembly instructions failed
		1: writing assembly instructions was successful


***************************************************************************************************************
*/
	int16_t val = 0;
	uint8_t result = 0;

	char sysInit[] = "Sys.init";

	// output file needs to be opened
	if (pFile != NULL) {
		val = fprintf(pFile, "%s\n", "//Bootstrap code\n@256\nD=A\n@SP\nM=D\n\n//call Sys.init 0");
		result = WriteCall(sysInit, 0, outputBuffer);
	}

	// check for errors in encoding and buffer overflow
	// fprintf return a negative value if something went wrong
	if (	(val >= 0)
		&& (result != 0)
	) {
		fprintf(pFile, "%s\n", outputBuffer);
		return 1;
	} else {
		printf("Error encoding\n");
		return 0;
	}
}
/*
***************************************************************************************************************
	End WriteInit
***************************************************************************************************************
*/

// TODO: let WriteCommand also handle opening file?
void WriteCommand(FILE* pFile, char* comment, char* fileName,  E_commandType command) {
/*!
***************************************************************************************************************

	\description
		Function generates assembly code for the VM command and writes it to the output file

	\param[out]		pFile			Pointer to output file
	\param[in]		comment		Pointer to comment string
	\param[in]		fileName		Pointer to filename
	\param[in]		command		VM command to assemble

	\note
		- comment parameter is output as a C-style comment in the output file just before the generated assembly
		  for the specified command (could be helpful for debugging)
		- fileName is used to generate file specific labels for the assembly code output

***************************************************************************************************************
*/
	// output file needs to be opened
	if (pFile != NULL) {
		fprintf(pFile, "//%s\n", comment);

		// WHY NOT CALL PARSER HERE??

		// get parsed data from parser
		E_memorySegment memorySegment = GetMemorySegment();
		uint16_t value = GetValue();
		char* name = GetName();

		ClearOutputBuffer();

		uint8_t result = 0;

		switch (command) {
		case CT_ADD:
		case CT_SUB:
		case CT_NEG:
		case CT_EQ:
		case CT_GT:
		case CT_LT:
		case CT_AND:
		case CT_OR:
		case CT_NOT:
			result = WriteArithmetic(command, outputBuffer);
			break;
		case CT_PUSH:
			result = WritePush(memorySegment, value, fileName, outputBuffer);
			break;
		case CT_POP:
			result = WritePop(memorySegment, value, fileName, outputBuffer);
			break;
		case CT_LABEL:
			result = WriteLabel(name, fileName, outputBuffer);
			break;
		case CT_GOTO:
			result = WriteGoto(name, fileName, outputBuffer);
			break;
		case CT_IFGOTO:
			result = WriteIfGoto(name, fileName, outputBuffer);
			break;
		case CT_FUNCTION:
			result = WriteFunction(name, value, outputBuffer);
			break;
		case CT_CALL:
			result = WriteCall(name, value, outputBuffer);
			break;
		case CT_RETURN:
			result = WriteReturn(outputBuffer);
			break;
		default:
			break;
		}

		// output generated code to the output file
		if (result != 0) {
			fprintf(pFile, "%s\n", outputBuffer);
		} else {
			printf("Error encoding\n");
		}
	}
}
/*
***************************************************************************************************************
	End WriteCommand
***************************************************************************************************************
*/

static uint8_t WriteArithmetic(E_commandType command, char* output) {
/*!
***************************************************************************************************************

	\description
		Function generated assembly code for the Arithmetic VM commands and writes it to a output buffer

	\param[in]		command		VM command to assemble
	\param[out]		output		Pointer to output buffer

	\returns
		0: writing assembly instruction failed
		1: writing assembly instructions was successful

***************************************************************************************************************
*/

	int16_t val = 0;
	static uint32_t labelCounter = 0;

	switch (command) {
	case CT_ADD:
		val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\n@R13\nM=D\n%s\n@R13\nD=D+M\n%s\n", POP_D, POP_D, PUSH_D);
		break;
	case CT_AND:
		val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\n@R13\nM=D\n%s\n@R13\nD=D&M\n%s\n", POP_D, POP_D, PUSH_D);
		break;
	case CT_EQ:
		val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\n@R13\nM=D\n%s\n@R13\nD=D-M\n@true%d\nD;JEQ\nD=0\n@end%d\n0;JMP\n(true%d)\nD=-1\n(end%d)\n%s\n"
																, POP_D, POP_D, labelCounter, labelCounter, labelCounter, labelCounter, PUSH_D);
		labelCounter++;
		break;
	case CT_GT:
		val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\n@R13\nM=D\n%s\n@R13\nD=D-M\n@true%d\nD;JGT\nD=0\n@end%d\n0;JMP\n(true%d)\nD=-1\n(end%d)\n%s\n"
																, POP_D, POP_D, labelCounter, labelCounter, labelCounter, labelCounter, PUSH_D);
		labelCounter++;
		break;
	case CT_LT:
		val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\n@R13\nM=D\n%s\n@R13\nD=D-M\n@true%d\nD;JLT\nD=0\n@end%d\n0;JMP\n(true%d)\nD=-1\n(end%d)\n%s\n"
																, POP_D, POP_D, labelCounter, labelCounter, labelCounter, labelCounter, PUSH_D);
		labelCounter++;
		break;
	case CT_NEG:
		val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\nD=-D\n%s\n", POP_D, PUSH_D);
		break;
	case CT_NOT:
		val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\nD=!D\n%s\n", POP_D, PUSH_D);
		break;
	case CT_OR:
		val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\n@R13\nM=D\n%s\n@R13\nD=D|M\n%s\n", POP_D, POP_D, PUSH_D);
		break;
	case CT_SUB:
		val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\n@R13\nM=D\n%s\n@R13\nD=D-M\n%s\n", POP_D, POP_D, PUSH_D);
		break;
	default:
		break;
	}

	// check for errors in encoding and buffer overflow
	// snprintf return a negative value if something went wrong while encoding the string
	if (	(val >= 0)
		&& (val < MAX_OUTPUT_LENGTH)
	) {
		return 1;
	} else {
		return 0;
	}
}
/*
***************************************************************************************************************
	End WriteArithmetic
***************************************************************************************************************
*/

static uint8_t WritePush(E_memorySegment memorySegment, uint16_t index, char* fileName, char* output) {
/*!
***************************************************************************************************************

	\description
		Function generates assembly code for the PUSH VM commands and writes it to a output buffer

	\param[in]		memorySegment	Memory Segment that is used to PUSH a value to
	\param[in]		index				Push the value of segment[index] onto the stack
	\param[in]		fileName			Pointer to filename
	\param[out]		output			Pointer to output buffer

	\returns
		0: writing assembly instruction failed
		1: writing assembly instructions was successful

	\note
		- fileName is used to generate file specific labels for the assembly code output
		- if MS_CONSTANT is the memory segment the index parameter is used as the value to PUSH onto the stack

***************************************************************************************************************
*/
	int16_t val = 0;

	switch (memorySegment) {
	case MS_LOCAL:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"@%d\nD=A\n@%s\nA=M\nA=D+A\nD=M\n%s\n", index, "LCL", PUSH_D);
		break;
	case MS_ARGUMENT:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"@%d\nD=A\n@%s\nA=M\nA=D+A\nD=M\n%s\n", index, "ARG", PUSH_D);
		break;
	case MS_THIS:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"@%d\nD=A\n@%s\nA=M\nA=D+A\nD=M\n%s\n", index, "THIS", PUSH_D);
		break;
	case MS_THAT:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"@%d\nD=A\n@%s\nA=M\nA=D+A\nD=M\n%s\n", index, "THAT", PUSH_D);
		break;
	case MS_CONSTANT:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"@%d\nD=A\n%s\n", index, PUSH_D);
		break;
	case MS_STATIC:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"@%s.%d\nD=M\n%s\n", fileName, index, PUSH_D);
		break;
	case MS_POINTER:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"@%d\nD=A\n@%s\nA=D+A\nD=M\n%s\n", index, "3", PUSH_D);
		break;
	case MS_TEMP:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"@%d\nD=A\n@%s\nA=D+A\nD=M\n%s\n", index, "5", PUSH_D);
		break;
	default:
		break;
	}

	// check for errors in encoding and buffer overflow
	// snprintf return a negative value if something went wrong while encoding the string
	if (	(val >= 0)
		&& (val < MAX_OUTPUT_LENGTH)
	) {
		return 1;
	} else {
		return 0;
	}
}
/*
***************************************************************************************************************
	End WritePush
***************************************************************************************************************
*/

static uint8_t WritePop(E_memorySegment memorySegment, uint16_t index, char* fileName, char* output) {
/*!
***************************************************************************************************************

	\description
		Function generates assembly code for the POP VM commands and writes it to a output buffer

	\param[in]		memorySegment	Memory Segment that is used to POP a value from
	\param[in]		index				POP the top stack value and store int in segment[index]
	\param[in]		fileName			Pointer to filename
	\param[out]		output			Pointer to output buffer

	\returns
		0: writing assembly instruction failed
		1: writing assembly instructions was successful

	\note
		- fileName is used to generate file specific labels for the assembly code output

***************************************************************************************************************
*/
	int16_t val = 0;

	switch (memorySegment) {
	case MS_LOCAL:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"%s\n@R13\nM=D\n@%d\nD=A\n@%s\nA=M\nD=D+A\n@R14\nM=D\n@R13\nD=M\n@R14\nA=M\nM=D\n"
																	, POP_D, index, "LCL");
		break;
	case MS_ARGUMENT:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"%s\n@R13\nM=D\n@%d\nD=A\n@%s\nA=M\nD=D+A\n@R14\nM=D\n@R13\nD=M\n@R14\nA=M\nM=D\n"
																	, POP_D, index, "ARG");
		break;
	case MS_THIS:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"%s\n@R13\nM=D\n@%d\nD=A\n@%s\nA=M\nD=D+A\n@R14\nM=D\n@R13\nD=M\n@R14\nA=M\nM=D\n"
																	, POP_D, index, "THIS");
		break;
	case MS_THAT:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"%s\n@R13\nM=D\n@%d\nD=A\n@%s\nA=M\nD=D+A\n@R14\nM=D\n@R13\nD=M\n@R14\nA=M\nM=D\n"
																	, POP_D, index, "THAT");
		break;
	case MS_STATIC:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"%s\n@%s.%d\nM=D\n", POP_D, fileName, index);
		break;
	case MS_POINTER:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"%s\n@R13\nM=D\n@%d\nD=A\n@%s\nD=D+A\n@R14\nM=D\n@R13\nD=M\n@R14\nA=M\nM=D\n"
																	, POP_D, index, "3");
		break;
	case MS_TEMP:
		val = snprintf(output, MAX_OUTPUT_LENGTH, 	"%s\n@R13\nM=D\n@%d\nD=A\n@%s\nD=D+A\n@R14\nM=D\n@R13\nD=M\n@R14\nA=M\nM=D\n"
																	, POP_D, index, "5");
		break;
	default:
		break;
	}

	// check for errors in encoding and buffer overflow
	// snprintf return a negative value if something went wrong while encoding the string
	if (	(val >= 0)
		&& (val < MAX_OUTPUT_LENGTH)
	) {
		return 1;
	} else {
		return 0;
	}
}
/*
***************************************************************************************************************
	End WritePop
***************************************************************************************************************
*/

static uint8_t WriteLabel(char* labelName,  char* fileName, char* output) {
/*!
***************************************************************************************************************

	\description
		Function generates assembly code for the Label VM command and writes it to a output buffer

	\param[in]		labelName	Pointer to labelname
	\param[in]		fileName		Pointer to filename
	\param[out]		output		Pointer to output buffer

	\returns
		0: writing assembly instruction failed
		1: writing assembly instructions was successful

	\note
		- fileName is used to generate file specific labels for the assembly code output

***************************************************************************************************************
*/
	int16_t val = 0;

	val = snprintf(output, MAX_OUTPUT_LENGTH, "(%s$%s)\n", fileName, labelName);

	// check for errors in encoding and buffer overflow
	// snprintf return a negative value if something went wrong while encoding the string
	if (	(val >= 0)
		&& (val < MAX_OUTPUT_LENGTH)
	) {
		return 1;
	} else {
		return 0;
	}
}
/*
***************************************************************************************************************
	End WriteLabel
***************************************************************************************************************
*/

static uint8_t WriteGoto(char* labelName,  char* fileName, char* output) {
/*!
***************************************************************************************************************

	\description
		Function generates assembly code for the Goto VM command and writes it to a output buffer

	\param[in]		labelName	Pointer to labelname
	\param[in]		fileName		Pointer to filename
	\param[out]		output		Pointer to output buffer

	\returns
		0: writing assembly instruction failed
		1: writing assembly instructions was successful

	\note
		- fileName is used to generate file specific labels for the assembly code output

***************************************************************************************************************
*/
	int16_t val = 0;

	val = snprintf(output, MAX_OUTPUT_LENGTH, "@%s$%s\n0;JMP\n", fileName, labelName);

	// check for errors in encoding and buffer overflow
	// snprintf return a negative value if something went wrong while encoding the string
	if (	(val >= 0)
		&& (val < MAX_OUTPUT_LENGTH)
	) {
		return 1;
	} else {
		return 0;
	}
}
/*
***************************************************************************************************************
	End WriteGoto
***************************************************************************************************************
*/

static uint8_t WriteIfGoto(char* labelName,  char* fileName, char* output) {
/*!
***************************************************************************************************************

	\description
		Function generates assembly code for the IfGoto VM command and writes it to a output buffer

	\param[in]		labelName	Pointer to labelname
	\param[in]		fileName		Pointer to filename
	\param[out]		output		Pointer to output buffer

	\returns
		0: writing assembly instruction failed
		1: writing assembly instructions was successful

	\note
		- fileName is used to generate file specific labels for the assembly code output

***************************************************************************************************************
*/
	int16_t val = 0;

	val = snprintf(output, MAX_OUTPUT_LENGTH, "%s\n@%s$%s\nD;JNE\n", POP_D, fileName, labelName);

	// check for errors in encoding and buffer overflow
	// snprintf return a negative value if something went wrong while encoding the string
	if (	(val >= 0)
		&& (val < MAX_OUTPUT_LENGTH)
	) {
		return 1;
	} else {
		return 0;
	}
}
/*
***************************************************************************************************************
	End WriteIfGoto
***************************************************************************************************************
*/

static uint8_t WriteFunction(char* labelName, uint8_t numLocals, char* output) {
/*!
***************************************************************************************************************

	\description
		Function generates assembly code for the Function VM command and writes it to a output buffer

	\param[in]		labelName	Pointer to labelname
	\param[in]		numLocals	The number of local variables the function uses
	\param[out]		output		Pointer to output buffer

	\returns
		0: writing assembly instruction failed
		1: writing assembly instructions was successful

	\note
		- fileName is used to generate file specific labels for the assembly code output

***************************************************************************************************************
*/
	int16_t val = 0;
	val = snprintf(output, MAX_OUTPUT_LENGTH, "(%s)\n", labelName);

	// check for errors in encoding and buffer overflow
	// snprintf return a negative value if something went wrong while encoding the string
	if (	(val >= 0)
		&& (val < MAX_OUTPUT_LENGTH)
	) {
		for (uint8_t i = 0; i < numLocals; i++) {
			strcat(output, "//PUSH 0 on stack for local variable\n");
			strcat(output, "D=0\n@SP\nA=M\nM=D\n@SP\nM=M+1\n");
		}
		return 1;
	} else {
		return 0;
	}
}
/*
***************************************************************************************************************
	End WriteFunction
***************************************************************************************************************
*/

static uint8_t WriteCall(char* labelName, uint8_t numParams, char* output) {
/*!
***************************************************************************************************************

	\description
		Function generates assembly code for the Call VM command and writes it to a output buffer

	\param[in]		labelName	Pointer to labelname
	\param[in]		numLocals	The number of paramters the function uses
	\param[out]		output		Pointer to output buffer

	\returns
		0: writing assembly instruction failed
		1: writing assembly instructions was successful

***************************************************************************************************************
*/
	int16_t val = 0;
	static uint32_t labelCounter = 0;

	val = snprintf(output, MAX_OUTPUT_LENGTH, "@%s_return%d\nD=A\n%s\n@LCL\nD=M\n%s\n@ARG\nD=M\n%s\n@THIS\nD=M\n%s\n@THAT\nD=M\n%s\n@SP\nD=M\n"
															"@5\nD=D-A\n@%d\nD=D-A\n@ARG\nM=D\n@SP\nD=M\n@LCL\nM=D\n@%s\n0;JMP\n(%s_return%d)\n"
															, labelName, labelCounter, PUSH_D, PUSH_D, PUSH_D, PUSH_D, PUSH_D, numParams, labelName, labelName
															, labelCounter);

	labelCounter++;

	// check for errors in encoding and buffer overflow
	// snprintf return a negative value if something went wrong while encoding the string
	if (	(val >= 0)
		&& (val < MAX_OUTPUT_LENGTH)
	) {
		return 1;
	} else {
		return 0;
	}
}
/*
***************************************************************************************************************
	End WriteCall
***************************************************************************************************************
*/

static uint8_t WriteReturn(char* output) {
/*!
***************************************************************************************************************

	\description
		Function generated assembly code for the Return VM command and writes it to a output buffer

	\param[out]		output		Pointer to output buffer

	\returns
		0: writing assembly instruction failed
		1: writing assembly instructions was successful


***************************************************************************************************************
*/
	int16_t val = 0;

	val = snprintf(output, MAX_OUTPUT_LENGTH, "@LCL\nD=M\n@R13\nM=D\n@5\nD=D-A\nA=D\nD=M\n@R14\nM=D\n%s\n@ARG\nA=M\nM=D\n@ARG\nD=M\n@SP\nM=D+1\n"
															"@R13\nD=M\n@1\nD=D-A\nA=D\nD=M\n@THAT\nM=D\n@R13\nD=M\n@2\nD=D-A\nA=D\nD=M\n@THIS\nM=D\n@R13\nD=M\n"
															"@3\nD=D-A\nA=D\nD=M\n@ARG\nM=D\n@R13\nD=M\n@4\nD=D-A\nA=D\nD=M\n@LCL\nM=D\n@R14\nA=M\n0;JMP\n"
															, POP_D);


	// check for errors in encoding and buffer overflow
	// snprintf return a negative value if something went wrong while encoding the string
	if (	(val >= 0)
		&& (val < MAX_OUTPUT_LENGTH)
	) {
		return 1;
	} else {
		return 0;
	}
}
/*
***************************************************************************************************************
	End WriteReturn
***************************************************************************************************************
*/




/*
***************************************************************************************************************
	TEST CODE
***************************************************************************************************************
*/

// TODO ADD MODULE TESTCODE

/*! \file
***************************************************************************************************************
file name:					parser.c
*	\copyright				FourE
*	\brief					parser source file
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

#include "parser.h"
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // atoi

/*
***************************************************************************************************************
	LOCAL DEFINES
***************************************************************************************************************
*/

#define NO_TOKENS_FOUND					(0)
#define MAX_TOKEN_PARTS					(3)
#define MAX_VMCOMMAND_STRING_LEN		(9) // max string length of string (including '\0') in table vmCommands
#define MAX_MEMSEGMENT_STRING_LEN	(9) // max string length of string (including '\0') in table vmMemorySegments

/*
***************************************************************************************************************
	LOCAL TYPEDEFS
***************************************************************************************************************
*/

typedef struct {
	char vmCommandString[MAX_VMCOMMAND_STRING_LEN];
	E_commandType command;
} T_vmCommandString;

typedef struct {
	char memorySegmentString[MAX_MEMSEGMENT_STRING_LEN];
	E_memorySegment memorySegment;
} T_vmMemorySegmentString;

/*
***************************************************************************************************************
	LOCAL FUNCTION PROTOTYPES
***************************************************************************************************************
*/

static E_commandType ParseCommandType(const char* input);
static E_memorySegment ParseMemorySegment(const char* input);
static void ClearCurrentCommand(void);

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

static const T_vmCommandString vmCommands[] = {
	 {"add"			,CT_ADD			}
	,{"sub"			,CT_SUB			}
	,{"neg"			,CT_NEG			}
	,{"eq"			,CT_EQ			}
   ,{"gt"			,CT_GT			}
	,{"lt"			,CT_LT			}
	,{"and"			,CT_AND			}
	,{"or"			,CT_OR			}
	,{"not"			,CT_NOT			}
	,{"push"			,CT_PUSH			}
	,{"pop"			,CT_POP			}
	,{"label"		,CT_LABEL		}
	,{"goto"			,CT_GOTO			}
	,{"if-goto"		,CT_IFGOTO		}
	,{"function"	,CT_FUNCTION	}
	,{"call"			,CT_CALL			}
	,{"return"		,CT_RETURN		}
};


#define MAX_VM_COMMANDS (sizeof(vmCommands) / sizeof(vmCommands[0]))

static const uint8_t maxVMCommands = MAX_VM_COMMANDS;


static const T_vmMemorySegmentString vmMemorySegments[] = {
	 {"local"		,MS_LOCAL			}
	,{"argument"	,MS_ARGUMENT		}
	,{"this"			,MS_THIS				}
	,{"that"			,MS_THAT				}
	,{"constant"	,MS_CONSTANT		}
	,{"static"		,MS_STATIC			}
	,{"pointer"		,MS_POINTER			}
	,{"temp"			,MS_TEMP				}
};


#define MAX_VM_MEMORY_SEGMENTS (sizeof(vmMemorySegments) / sizeof(vmMemorySegments[0]))

static const uint8_t maxVMMemorySegments = MAX_VM_MEMORY_SEGMENTS;

static T_vmCommand currentCommand;


/*
***************************************************************************************************************
	IMPLEMENTATION
***************************************************************************************************************
*/

void InitParser(void) {
/*!
***************************************************************************************************************

	\description
		Function initializes the parser

***************************************************************************************************************
*/
	ClearCurrentCommand();
}
/*
***************************************************************************************************************
	End InitParser
***************************************************************************************************************
*/

uint8_t ParseCommand(char* input) {
/*!
***************************************************************************************************************

	\description
		Function parses input string (command) and dissects it into specific parts like:
		- commandType
		- memorySegment
		- value

	\param[in]		input		Pointer to input string

	\returns
			0: parsing failed
			1: parsing was successful

	\note
		- make sure that input is not NULL
		- make sure input string is nul terminated with '\0

		- !!! the input string is MODIFIED by the function (strtok modifies it) !!!

***************************************************************************************************************
*/
	assert(input != NULL);

	char* token = NULL;
	uint8_t count = NO_TOKENS_FOUND;

	ClearCurrentCommand();

	// find first token
	token = strtok(input, " ");

	while (	(token != NULL)
			&&	(count <= MAX_TOKEN_PARTS)
	) {
		count++; // used to keep track of current token in input string
		printf("%s\n", token);

		// parse and store token based on "position" in input string
		switch (count) {
		case 1:
			// command
			currentCommand.commandType = ParseCommandType(token);
			break;
		case 2:
			// argument 1
			if (	(currentCommand.commandType == CT_PUSH)
				|| (currentCommand.commandType == CT_POP)
			) {
				currentCommand.argument1.memorySegment = ParseMemorySegment(token);
			} else {
				// assume it is: label, goto, if-goto, function, call
				currentCommand.argument1.name = token;
			}
			break;
		case 3:
			// argument 2
			currentCommand.value = atoi(token);
			break;
		default:
			// not handled
			break;
		}

		// get next token
		token = strtok(NULL, " ");
	}

	//printf("count = %d\n", count);
	if (	(count <= MAX_TOKEN_PARTS)
		&& (count > NO_TOKENS_FOUND)
	) {
		return 1;
	} else {
		ClearCurrentCommand(); // clear command struct just to be sure
		return 0;
	}
}
/*
***************************************************************************************************************
	End ParseCommand
***************************************************************************************************************
*/

E_commandType GetCommandType(void) {
/*!
***************************************************************************************************************

	\description
		Function returns the commandType of the current command that was parsed

	\returns
		CommandType of the current command that was parsed

***************************************************************************************************************
*/

	return currentCommand.commandType;
}
/*
***************************************************************************************************************
	End GetCommandType
***************************************************************************************************************
*/

uint16_t GetValue(void) {
/*!
***************************************************************************************************************

	\description
		Function returns the integer value of the current command that was parsed

	\returns
		Integer value of the current command that was parsed

***************************************************************************************************************
*/
	return currentCommand.value;
}
/*
***************************************************************************************************************
	End GetValue
***************************************************************************************************************
*/

E_memorySegment GetMemorySegment(void) {
/*!
***************************************************************************************************************

	\description
		Function returns the memory segment of the current command that was parsed

	\returns
		Memory segment of the current command that was parsed

***************************************************************************************************************
*/
	return currentCommand.argument1.memorySegment;
}
/*
***************************************************************************************************************
	End GetMemorySegment
***************************************************************************************************************
*/

char* GetName(void) {
/*!
***************************************************************************************************************

	\description
		Function returns the name (string) of the current command that was parsed

	\returns
		Name (string) of the current command that was parsed

***************************************************************************************************************
*/
	return currentCommand.argument1.name;
}
/*
***************************************************************************************************************
	End GetName
***************************************************************************************************************
*/

static void ClearCurrentCommand(void) {
/*!
***************************************************************************************************************

	\description
		Function reinitializes the fields of the currentCommand

***************************************************************************************************************
*/
	memset(&currentCommand, 0 , sizeof(T_vmCommand));
}
/*
***************************************************************************************************************
	End ClearCurrentCommand
***************************************************************************************************************
*/

static E_commandType ParseCommandType(const char* input) {
/*!
***************************************************************************************************************

	\description
		Function tries to find the commandType by parsing the command token

	\param[in]		input		Pointer to input string (commandType token)

	\returns
		commandType of the parsed command

***************************************************************************************************************
*/
	uint8_t i = 0;
	uint8_t found = 0;

	while (	(found == 0)
			&& (i < maxVMCommands)
	) {
		if (strcmp(vmCommands[i].vmCommandString, input) == 0) {
			found = 1;
		} else {
			i++;
		}
	}

	if (found != 0) {
		return vmCommands[i].command;
	} else {
		return CT_UNKNOWN;
	}
}
/*
***************************************************************************************************************
	End ParseCommandType
***************************************************************************************************************
*/

static E_memorySegment ParseMemorySegment(const char* input) {
/*!
***************************************************************************************************************

	\description
		Function tries to find the memorySegment by parsing the memorySegment token

	\param[in]		input		Pointer to input string (memorySegment token)

	\returns
		memorySegment of the parsed command

***************************************************************************************************************
*/
	uint8_t i = 0;
	uint8_t found = 0;

	while (	(found == 0)
			&& (i < maxVMMemorySegments)
	) {
		if (strcmp(vmMemorySegments[i].memorySegmentString, input) == 0) {
			found = 1;
		} else {
			i++;
		}
	}

	if (found != 0) {
		return vmMemorySegments[i].memorySegment;
	} else {
		return MS_UNKNOWN;
	}
}
/*
***************************************************************************************************************
	End ParseMemorySegment
***************************************************************************************************************
*/



/*
***************************************************************************************************************
	TEST CODE
***************************************************************************************************************
*/

// TODO ADD MODULE TESTCODE

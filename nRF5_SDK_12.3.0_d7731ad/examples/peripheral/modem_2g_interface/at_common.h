#ifndef __MXN003_CMD_H
#define __MXN003_CMD_H

#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define COMMAND_LINE_SIZE    (180)
#define NULL_TERMINATOR_LENGTH (1)
#define MAX_UART_LEN	128 

#define COMMA_UNIT_MAX 10


typedef enum
{
	CUSTOM_RSP_ERROR = -1,
	CUSTOM_RSP_OK = 0,
	CUSTOM_RSP_LATER //add by aihua
} custom_rsp_type_enum;


typedef struct 
{
	short  position;
	short  length;
	char   character[COMMAND_LINE_SIZE + NULL_TERMINATOR_LENGTH];
} custom_cmdLine;
	
	
typedef enum
{
	CUSTOM_WRONG_MODE,
	CUSTOM_SET_OR_EXECUTE_MODE,
	CUSTOM_READ_MODE,
	CUSTOM_TEST_MODE,
	CUSTOM_ACTIVE_MODE
} custom_cmd_mode_enum;


typedef struct
{
		short  position;
    uint8_t   	 part;
		char     		 rcv_msg[MAX_UART_LEN];
    char       	 *pars[COMMA_UNIT_MAX];
		uint16_t      rcv_length;
} cmd_data_struct;

typedef enum
{
    CM_Main,
    CM_Par1,
    CM_Par2,
    CM_Par3,
    CM_Par4,
    CM_Par5,
    CM_Par6,
    CM_Par7,
    CM_Par8,
    CMD_MAX_Pars
} Cmd_Pars;

#ifdef __cplusplus
}
#endif

#endif


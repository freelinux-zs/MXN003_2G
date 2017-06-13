#ifndef __MXN003_CMD_H
#define __MXN003_CMD_H

#include <stdint.h>
#include <string.h>
#define  MAX_NMEA_LENGTH 128

typedef struct {
	int count;
	char buffer[MAX_NMEA_LENGTH];
}NmeaReader;

typedef char *Token;
typedef struct{
	int count;
	Token tokens[MAX_NMEA_LENGTH];
}NmeaTokenizer;

#define CM_PARAM_LONG_LEN_MAX  128


#define GPS_COMMA_UNIT_MAX 10

typedef enum
{
    CM_AT,              
    CM_SMS,             
    CM_ONLINE,          
    CM_ONLINE2,         
    CM_PARAM_CMD,
    CM_TEST_ONLINE,
    CM_VS_AT,
    CM_VS_EX,
    CMD_MAX_TYPE
} Cmd_Type;

typedef struct
{
    unsigned char           reply_character_encode; // 0 ASCII   1 UTF-16BE   2 GB2312
    unsigned char            num_type;               // ????//145?? 160 ??//129 10086 ??
    unsigned char            stamp[4];               // ???,????????????????????????
    Cmd_Type            cm_type;
} cmd_return_struct;

typedef enum
{
    Error_110,
    CMD_Cmd_Ret_Code
} Cmd_Ret_Code;

typedef struct
{
    uint8_t             part;
    char                rcv_msg_source[CM_PARAM_LONG_LEN_MAX];
    char                rcv_msg[CM_PARAM_LONG_LEN_MAX];
    uint16_t             rcv_length;
    char                *pars[GPS_COMMA_UNIT_MAX];
    Cmd_Ret_Code        rsp_code;
    char                rsp_msg[CM_PARAM_LONG_LEN_MAX];
    uint16_t                 rsp_length;
    uint8_t           supercmd;               
    uint8_t           cmd_pwd_type;           
    uint8_t           character_encode;       
    cmd_return_struct   return_sms;
} CMD_DATA_STRUCT;

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

typedef void(*Func_CustRecvPtr)(CMD_DATA_STRUCT*);

typedef struct
{
    const char           *cmd_string;
    Func_CustRecvPtr     func_ptr;
} cust_recv_struct;


extern void uart_init(void);
#endif


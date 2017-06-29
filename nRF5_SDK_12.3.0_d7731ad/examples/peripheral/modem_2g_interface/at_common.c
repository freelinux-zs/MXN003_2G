#include "at_common.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "interface.h"
#include "at_command_custom_hdlr.h"



typedef struct
{
	char *commandString;
	custom_rsp_type_enum (*commandFunc)(custom_cmdLine *commandBuffer_p);
} custom_atcmd;


custom_cmd_mode_enum custom_find_cmd_mode(custom_cmdLine *cmd_line)
{
    custom_cmd_mode_enum result;
		uart_putbyte("cmd_line->position = %d, cmd_line->length = %d",cmd_line->position,cmd_line->length);
   // if (cmd_line->position < cmd_line->length - 1) //modfiy by xuzhoubin for no '\r\n'
		if (cmd_line->position < cmd_line->length)
    {
        switch (cmd_line->character[cmd_line->position])
        {
            case '?':  /* AT+...? */
            {
                cmd_line->position++;
                result = CUSTOM_READ_MODE;
                break;
            }
            case '=':  /* AT+...= */
            {
                cmd_line->position++;
                if ((cmd_line->position < cmd_line->length - 1 ) &&
                    (cmd_line->character[cmd_line->position] == '?'))
                {
                    cmd_line->position++;
                    result = CUSTOM_TEST_MODE;
                }
                else
                {
                    result = CUSTOM_SET_OR_EXECUTE_MODE;
                }
                break;
            }
            default:  /* AT+... */
            {
                result = CUSTOM_ACTIVE_MODE;
                break;
            }
        }
    }
    else
    {
        result = CUSTOM_ACTIVE_MODE;
    }
    return (result);
}




//static void fun_toUpper(char *buf)
//{
//    char *ptr = buf;
//    while(*ptr)
//    {
//        if(*ptr >= 'a' && *ptr <= 'z')
//        {
//            *ptr -= 0x20;
//        }
//        ptr++;
//    }
//}

static int fun_str_analyse(char *str_data, char **tar_data, int limit, char startChar, char *endChars, char splitChar)
{
    static char *blank = "";
    int len, i = 0, j = 0, status = 0;
    char *p;
    if(str_data == NULL || tar_data == NULL)
    {
        return -1;
    }

    len = strlen(str_data);
		if(str_data[len - 1] == '\n'){
			len = len - 2;  //add by xuzhoubin
		}
    for(i = 0, j = 0, p = str_data; i < len; i++, p++)
    {
        if(status == 0 && (*p == startChar || startChar == NULL))
        {
            status = 1;
            if(j >= limit)
            {
                return -2;
            }
            if((startChar == NULL))
            {
                tar_data[j++] = p;
            }
            else if(*(p + 1) == splitChar)
            {
                tar_data[j++] = blank;
            }
            else
            {
                tar_data[j++] = p + 1;
            }
        }
        if(status == 0)
        {
            continue;
        }
        if(strchr(endChars, *p) != NULL)
        {
            *p = 0;
            break;
        }
        if(*p == splitChar)
        {
            *p = 0;
            if(j >= limit)
            {
                return -3;
            }
            if(strchr(endChars, *(p + 1)) != NULL || *(p + 1) == splitChar)
            {
                tar_data[j++] = blank;
            }
            else
            {
                tar_data[j++] = p + 1;
            }
        }
    }
    for(i = j; i < limit; i++)
    {
        tar_data[i] = blank;
    }
    return j;
}



static int cmd_analyse(cmd_data_struct * command)
{
	char        *data_ptr, split_ch = ',';
	int         cmd_Len, par_len;
	
	
	if(command == NULL || command->rcv_length < 1)
  {
        return -1;
  }
	//fun_toUpper(command->rcv_msg);
	data_ptr = &command->rcv_msg[command->position];//parse_sms_head(command->rcv_msg); 
	
	cmd_Len = strlen(data_ptr);

    if(data_ptr[cmd_Len - 3] == '#' && data_ptr[cmd_Len - 2] == 0x0D && data_ptr[cmd_Len - 1] == 0x0A)
    {
        data_ptr[cmd_Len - 3] = 0;
    }
    else if(data_ptr[cmd_Len - 2] == '#' && data_ptr[cmd_Len - 1] == 0x0D)
    {
        data_ptr[cmd_Len - 2] = 0;
    }
    else if(data_ptr[cmd_Len - 1] == '#')
    {
        data_ptr[cmd_Len - 1] = 0;
    }
   // else 
   // {
        //LOGHEX(M_CMD, LV6, data_ptr, cmd_Len);
    //}
			
		par_len = fun_str_analyse(data_ptr, command->pars, COMMA_UNIT_MAX, NULL, "\r\n", split_ch);

    if(par_len > COMMA_UNIT_MAX || par_len <= 0)
    {
        return -1;
    }
		
		if(par_len > COMMA_UNIT_MAX || par_len <= 0)
    {
        return -1;
    }
		
		if((par_len - CM_Main) > 0)
    {
        command->part = par_len - CM_Main;
    }
    else
    {
        command->part = 0;
    }	

	return 1;
}


cmd_data_struct at_get_at_para(custom_cmdLine *commandBuffer_p)
{
		static cmd_data_struct at_cmd = {0};
		if(commandBuffer_p->length && (commandBuffer_p->length <(COMMAND_LINE_SIZE + NULL_TERMINATOR_LENGTH)))
		{
			memset(&at_cmd, 0, sizeof(cmd_data_struct)); 
			memcpy(&at_cmd.rcv_msg, commandBuffer_p->character, commandBuffer_p->length);	

			at_cmd.rcv_length = commandBuffer_p->length;
			at_cmd.position   = commandBuffer_p->position;
			uart_putbyte("at_cmd.rcv_length= %d ,at_cmd.position  = %d ,at_cmd.rcv_msg = %s",at_cmd.rcv_length,at_cmd.position,at_cmd.rcv_msg);
			cmd_analyse(&at_cmd);
		}
		return at_cmd;
}



static custom_rsp_type_enum custom_test_func(custom_cmdLine *commandBuffer_p)
{
    custom_cmd_mode_enum result;
    custom_rsp_type_enum ret_value  = CUSTOM_RSP_ERROR;
    result = custom_find_cmd_mode(commandBuffer_p);		
    switch (result)
    {
        case CUSTOM_READ_MODE:
            ret_value = CUSTOM_RSP_OK;
            break;
        case CUSTOM_ACTIVE_MODE:
            ret_value = CUSTOM_RSP_OK;
            break;
        case CUSTOM_SET_OR_EXECUTE_MODE:
            ret_value = CUSTOM_RSP_OK;
            break;
        case CUSTOM_TEST_MODE:
            ret_value = CUSTOM_RSP_OK;
            break;    
        default:
            ret_value = CUSTOM_RSP_ERROR;
            break;
	}
    return ret_value;
}




const custom_atcmd custom_cmd_table[ ] =
{    
    {"AT%CUSTOM",custom_test_func},
		{"AT+LED",cusotm_led_test_func},
    {NULL, NULL}  // this lind should not be removed, it will be treat as 
};


bool mt2503_custom_common_hdlr(char *full_cmd_string)
{
    char buffer[MAX_UART_LEN+1];
    char *cmd_name, *cmdString;
    uint8_t index = 0; 
    uint16_t length;
    uint16_t i;
    custom_cmdLine command_line;
		cmd_name = buffer;
	
		length = strlen(full_cmd_string);
		length = length > MAX_UART_LEN ? MAX_UART_LEN : length;  
	
		while ((full_cmd_string[index] != '=' ) &&  //might be TEST command or EXE command
					(full_cmd_string[index] != '?' ) && // might be READ command
					(full_cmd_string[index] != 13 ) && //carriage return
				index < length)  
		{
				cmd_name[index] = full_cmd_string[index] ;
				index ++;
		}
		cmd_name[index] = '\0' ;   


    for (i = 0 ; custom_cmd_table[i].commandString != NULL; i++ )
    {
        cmdString = custom_cmd_table[i].commandString;
        if (strcmp(cmd_name, cmdString) == 0 )
        {
            strncpy(command_line.character, full_cmd_string, COMMAND_LINE_SIZE + NULL_TERMINATOR_LENGTH);
            command_line.character[COMMAND_LINE_SIZE] = '\0';
            command_line.length = strlen(command_line.character);
            command_line.position = index;
            if (custom_cmd_table[i].commandFunc(&command_line) == CUSTOM_RSP_OK) {
								//printf("\r\nOK\r\n");
								uart_putbyte("OK");
						}else{
							//	printf("\r\nERROR\r\n");
								uart_putbyte("ERROR");
            }
            return true;
        }
    }    

		
		return true;
}


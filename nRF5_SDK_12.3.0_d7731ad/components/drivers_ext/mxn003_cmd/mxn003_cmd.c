#include "sdk_common.h"
#include "app_uart.h"
#include "boards.h"
#include "mxn003_cmd.h"

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                1                                         /**< UART RX buffer size. */


void LED(CMD_DATA_STRUCT *cmd)
{
		printf(" \r\n len:%d, data:%s   [%d]%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|\r\n",
          cmd->rcv_length, cmd->rcv_msg_source, cmd->part, cmd->pars[0], cmd->pars[1], cmd->pars[2], cmd->pars[3], cmd->pars[4],
           cmd->pars[5], cmd->pars[6], cmd->pars[7],cmd->pars[8],cmd->pars[9]);
		if(strcmp(cmd->pars[0], "1"))
		{
				if(strcmp(cmd->pars[1], "ON")){
					nrf_gpio_pin_toggle(LED_2);
				}
		}
}

void LCD(CMD_DATA_STRUCT *cmd)
{
		printf(" len:%d, data:%s   [%d]%s|%s|%s|%s|%s |%s|%s|%s\n",
          cmd->rcv_length, cmd->rcv_msg_source, cmd->part, cmd->pars[0], cmd->pars[1], cmd->pars[2], cmd->pars[3], cmd->pars[4],
           cmd->pars[5], cmd->pars[6], cmd->pars[7]);
}


static cust_recv_struct logic_cmd_table[] =
{
    {"LED",    LED,                       },          //EADC
    {"LCD",    LCD,                       },          //EADC
    {" ", NULL}
};



static void nmea_read_init(NmeaReader *reader)
{
	memset(reader, 0, sizeof(NmeaReader));
}

static void nmea_tokenizer_init(NmeaTokenizer *tokenizer){
		memset(tokenizer, 0, sizeof(NmeaTokenizer));
}

static int nmea_reader_count(NmeaReader *reader){
		return reader->count;
}

static void nmea_tokenizer_set(NmeaTokenizer *tokenizer, Token nmeaToken, int index)
{
	tokenizer->tokens[index] = nmeaToken;
}

static char *nmea_reader_token(NmeaReader *reader, int n)
{
	return &reader->buffer[n];
}

static void nmea_tokenizer_set_count(NmeaTokenizer *tokenizer, int n){
	tokenizer->count = n;
}

static void nmea_reader_to_tokenizer(NmeaReader *reader, NmeaTokenizer *tokenizer)
{
	int i, counter = 0;
	for(i = 0; i < nmea_reader_count(reader); i++){
			Token token = (Token)nmea_reader_token(reader, i);
			nmea_tokenizer_set(tokenizer, token, counter);
			counter += 1;
	}
	nmea_tokenizer_set_count(tokenizer,counter);
}


static void nmea_reader_add(NmeaReader *reader, char c){
	int index = reader->count;
	reader->buffer[index] = c;
	index += 1;
	reader->count = index;
}

static Token nmea_tokenizer_get(NmeaTokenizer *tokenizer, int index){
	return tokenizer->tokens[index];
}

static void fun_toUpper(char *buf)
{
    char *ptr = buf;
    while(*ptr)
    {
        if(*ptr >= 'a' && *ptr <= 'z')
        {
            *ptr -= 0x20;
        }
        ptr++;
    }
}

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
    for(i = 0, j = 0, p = str_data; i < len; i++, p++)
    {
        if(status == 0 && (*p == startChar || startChar == NULL))
        {
            status = 1;
            if(j >= limit)
            {
                return -2;
            }
            if(startChar == NULL)
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

static char* parse_sms_head(char *data)
{
    char* SMS_str = NULL;
    char SMS_Head[15] = {"CM="};

    SMS_str = strstr(data, SMS_Head);

    if(SMS_str == NULL)
    {
        return data;
    }
    else
    {
        return (SMS_str +strlen(SMS_Head));
    }
}

static int cmd_analyse(CMD_DATA_STRUCT * command)
{
	char        *data_ptr, split_ch = ',';
	int         cmd_Len, par_len;
	if(command == NULL || command->rcv_msg == NULL || command->rcv_length < 1)
  {
        return -1;
  }
	memcpy(command->rcv_msg_source, command->rcv_msg, CM_PARAM_LONG_LEN_MAX);
	fun_toUpper(command->rcv_msg);
	data_ptr = parse_sms_head(command->rcv_msg); 
	cmd_Len = strlen(data_ptr);
	
    if(command->return_sms.cm_type == CM_SMS && data_ptr[cmd_Len - 1] != '#')
    {
        return -1;
    }
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
    else 
    {
        //LOGHEX(M_CMD, LV6, data_ptr, cmd_Len);
    }

   par_len = fun_str_analyse(data_ptr, command->pars, GPS_COMMA_UNIT_MAX, NULL, "\r\n", split_ch);

    if(par_len > GPS_COMMA_UNIT_MAX || par_len <= 0)
    {
        return -1;
    }
		
		if(par_len > GPS_COMMA_UNIT_MAX || par_len <= 0)
    {
        return -1;
    }
		
		if((par_len - CM_Par1) > 0)
    {
        command->part = par_len - CM_Par1;
    }
    else
    {
        command->part = 0;
    }	
	return 1;
}

static int logic_cmd(CMD_DATA_STRUCT * cmd)
{
	 int i, size, ret;
   size = sizeof(logic_cmd_table) / sizeof(cust_recv_struct);
	if(cmd->rcv_length >= CM_PARAM_LONG_LEN_MAX){
		return 1;
	}
	for(i = 0; i < size; i++){
		ret = strcmp(cmd->pars[0], logic_cmd_table[i].cmd_string);
		if(!ret){
			cmd->rsp_length = 0;
			memset(cmd->rsp_msg, 0, sizeof(cmd->rsp_msg));
			if(logic_cmd_table[i].func_ptr != NULL){
					logic_cmd_table[i].func_ptr(cmd);
					break;
			}		
		}
	}

	return 0;
}

static uint8_t command_operate(CMD_DATA_STRUCT * command)
{
	if(cmd_analyse(command))
	{
		if(logic_cmd(command)){
		return 0;
	}else{
		memset(command, 0, sizeof(CMD_DATA_STRUCT));
		return 0;
		}
	}
	
	return 0;
}

static void at_get_ctx(void *msg,int length, char type)
{

	CMD_DATA_STRUCT at_cmd = {0};

	 if(msg && length && (length < MAX_NMEA_LENGTH))
	 {	
		 memset(&at_cmd, 0, sizeof(CMD_DATA_STRUCT));  	
		 memcpy(&at_cmd.rcv_msg, msg, length);
		
		 at_cmd.rcv_length = length;
		 at_cmd.return_sms.cm_type =(Cmd_Type)type;
		 if(at_cmd.return_sms.cm_type == CM_AT)
		 {
			command_operate(&at_cmd);
			 
		 }else if(at_cmd.return_sms.cm_type == CM_VS_AT)
		 {
				//ÆäËûATÃüÁîÔ¤Áô
		 }
		 

	 }

}


static int cmd_parse(uint8_t*cmd_name, void * full_cmd_string, int length)
{
	char cm_t = 0xFF;
	
	fun_toUpper((char *) cmd_name);

	 if(strcmp((char *)cmd_name, "CM") == 0)
	 {
		 cm_t = CM_AT;
	 }else if(strcmp((char *)cmd_name, "OL") == 0){
		  cm_t = CM_VS_AT;
	 } else{
		printf("\r\n Inter no AT MODEM\r\n");
		return -1;	
	 }
	 at_get_ctx(full_cmd_string,length,cm_t);
	 return 1;
}

static int nmea_parser(NmeaTokenizer *tokenizer){
	uint16_t length;
	char buffer[20+1]; 
	char *cmd_name;
	uint8_t index = 0; 
	
	cmd_name = buffer;
	Token headToken = nmea_tokenizer_get(tokenizer,0);
	length = strlen(headToken);
	
	length = length > MAX_NMEA_LENGTH ? MAX_NMEA_LENGTH : length;   

	while ((headToken[index] != '=' ) &&  //might be TEST command or EXE command
        (headToken[index] != '?' ) && // might be READ command
        (headToken[index] != 13 ) && //carriage return
        index < length)  
    {
        cmd_name[index] = headToken[index] ;
        index ++;
    }
    cmd_name[index] = '\0' ;   //½âÎö=Ç°ÃæµÄ×Ö·û´®£¬ÅÐ¶ÏÊÇ·ñÎªATÃüÁî
		if (index <= 2)
        return -1;
	
    /* just a very basic example : customer can implement their own */
    cmd_name += 3;
    cmd_parse((uint8_t*)cmd_name, (uint8_t*)headToken,length);
		
	return 1;
}

static void uart_event_handle(app_uart_evt_t * p_event)
{
		static uint8_t data_array[256];
		static uint8_t index = 0;
		char ch ;
		NmeaReader reader;
		nmea_read_init(&reader);
		NmeaTokenizer tokenizer;
	
    switch (p_event->evt_type)
    {		
        case APP_UART_DATA_READY:
						UNUSED_VARIABLE(app_uart_get(&data_array[index]));
						index++;
						if ((data_array[index - 1] == '\n') || (index >= 256)){
							for( int i = 0; i < index; i++){
								ch = data_array[i];
								nmea_reader_add(&reader,ch);					
							}
				
							nmea_tokenizer_init(&tokenizer);
							nmea_reader_to_tokenizer(&reader, &tokenizer);
							nmea_parser(&tokenizer);			
							index = 0;
						}
            break;

        case APP_UART_COMMUNICATION_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_communication);
            break;

        case APP_UART_FIFO_ERROR:
            APP_ERROR_HANDLER(p_event->data.error_code);
            break;

        default:
            break;
    }
}

/**@brief  Function for initializing the UART module.
 */
/**@snippet [UART Initialization] */
void uart_init(void)
{
    uint32_t                     err_code;
    const app_uart_comm_params_t comm_params =
    {
        RX_PIN_NUMBER,
        TX_PIN_NUMBER,
        RTS_PIN_NUMBER,
        CTS_PIN_NUMBER,
        APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE_BAUDRATE_Baud115200
    };

    APP_UART_FIFO_INIT( &comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
    APP_ERROR_CHECK(err_code);
}

void uart_onoff(int8_t on)
{
	if(1 == on){
	 NRF_UART0->ENABLE        = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
   NRF_UART0->TASKS_STARTRX = 1;
   NRF_UART0->TASKS_STARTTX = 1;
	}else{
		NRF_UART0->TASKS_STOPTX = 1;
		NRF_UART0->TASKS_STOPRX = 1;
		NRF_UART0->ENABLE       = (UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos);
	}
}

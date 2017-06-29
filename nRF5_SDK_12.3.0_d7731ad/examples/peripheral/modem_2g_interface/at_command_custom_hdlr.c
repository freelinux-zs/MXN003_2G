#include "interface.h"
#include "at_common.h"
#include "at_fun.h"
#include <stdio.h>

#if 0  // //ÄÚ²¿²âÊÔ
custom_rsp_type_enum cusotm_led_test_func(custom_cmdLine *commandBuffer_p)
{
	custom_cmd_mode_enum result;
	custom_rsp_type_enum ret_value	= CUSTOM_RSP_ERROR;
	result = custom_find_cmd_mode(commandBuffer_p); 
	cmd_data_struct cmd;
	
	
	
	cmd = at_get_at_para(commandBuffer_p);
	
	for(int m =0 ; m < cmd.part; m++)
			uart_putbyte(" cmd.pard =%d cmd.pars[%d] = %s",cmd.part,m,cmd.pars[m]);
	
	uart_putbyte("result = %d",result);
	switch (result)
	{
			case CUSTOM_READ_MODE:
				ret_value = CUSTOM_RSP_OK;
				break;
			case CUSTOM_SET_OR_EXECUTE_MODE:
				ret_value = CUSTOM_RSP_OK;
				break;
			case CUSTOM_TEST_MODE:
				ret_value = CUSTOM_RSP_OK;
				break;
			case CUSTOM_ACTIVE_MODE:
				ret_value = CUSTOM_RSP_OK;
				break;
			default:
				ret_value = CUSTOM_RSP_ERROR;
				break;
	}
	
	return ret_value;
}
#endif

custom_rsp_type_enum ata_set_apn_return(custom_cmdLine *commandBuffer_p)
{
	custom_cmd_mode_enum result;
	custom_rsp_type_enum ret_value	= CUSTOM_RSP_ERROR;
	result = custom_find_cmd_mode(commandBuffer_p); 
	cmd_data_struct cmd;
	
	cmd = at_get_at_para(commandBuffer_p);
	
	switch (result)
	{
			case CUSTOM_SET_OR_EXECUTE_MODE:
				uart_putbyte("cmd.pars[0] = %s",cmd.pars[cmd.part - 1]);
				ret_value = CUSTOM_RSP_OK;
				break;
			default:
				ret_value = CUSTOM_RSP_ERROR;
				break;
	}
	
	return ret_value;
}


//****////

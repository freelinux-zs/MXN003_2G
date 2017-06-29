#ifndef __AT_COMMAND_CUSTOM_HDLR
#define  __AT_COMMAND_CUSTOM_HDLR
#include "at_common.h"

#ifdef __cplusplus
extern "C" {
#endif

//extern custom_rsp_type_enum cusotm_led_test_func(custom_cmdLine *commandBuffer_p);   //内部测试
extern custom_rsp_type_enum ata_set_apn_return(custom_cmdLine *commandBuffer_p);  
#ifdef __cplusplus
}
#endif

#endif


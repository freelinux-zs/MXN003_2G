#ifndef __AT_FUN_H
#define __AT_FUN_H

#include <stdint.h>
#include <string.h>
#include "at_common.h"

#ifdef __cplusplus
extern "C" {
#endif
	
extern cmd_data_struct at_get_at_para(custom_cmdLine *commandBuffer_p);
extern custom_cmd_mode_enum custom_find_cmd_mode(custom_cmdLine *cmd_line);
	
#ifdef __cplusplus
}
#endif

#endif


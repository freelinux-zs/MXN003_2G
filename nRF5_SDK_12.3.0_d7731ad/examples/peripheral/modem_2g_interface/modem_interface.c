/**
 * Copyright (c) 2009 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

/** @file
* @brief Example template project.
* @defgroup nrf_templates_example Example Template
*
*/

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nrf.h"
#include "nrf_gpio.h"
#include "nordic_common.h"
#include "nrf_delay.h"
#include "app_error.h"
#include "app_timer.h"
#include "app_util_platform.h"
#include "boards.h"
#include "bsp.h"
#include "app_uart.h"
#include <stdarg.h>
//#include "modem_interface.h"
#include "interface.h"


#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */


#define APP_TIMER_PRESCALER             0                                         
#define APP_TIMER_OP_QUEUE_SIZE         4   

APP_TIMER_DEF(uart_timer_id);
#define UART_IMTES_INTERVAL       APP_TIMER_TICKS(10, APP_TIMER_PRESCALER)
#define UART_BUFFER_SIZE (128)
static uint8_t data_array[UART_BUFFER_SIZE];
static uint8_t index = 0;
static uint8_t timer_start = 0;

void uart_putbyte(const char *fmt, ...);

static void uart_timeout_handler(void * p_context)
{
    UNUSED_PARAMETER(p_context);
		app_timer_stop(uart_timer_id);

		mt2503_custom_common_hdlr((char *)data_array);
		memset(data_array, 0, UART_BUFFER_SIZE);
		timer_start = 0;
		index = 0;
}

static void uart_event_handle(app_uart_evt_t * p_event)
{
    switch (p_event->evt_type)
    {
        case APP_UART_DATA_READY:
            UNUSED_VARIABLE(app_uart_get(&data_array[index]));
            index++;
					
						
						if(timer_start == 0){
							app_timer_start(uart_timer_id, UART_IMTES_INTERVAL, NULL);
							timer_start = 1;
						}		
						
						if((index >= (UART_BUFFER_SIZE)))
						{
							memset(data_array, 0, UART_BUFFER_SIZE);
							index = 0;
						} 
						
						if((data_array[index - 1] == '\n') && (timer_start == 1)){
							app_timer_stop(uart_timer_id);
							mt2503_custom_common_hdlr((char *)data_array);
							memset(data_array, 0, UART_BUFFER_SIZE);
							index = 0;
							timer_start = 0;
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


static void U_PutUARTBytes(uint8_t *data,uint16_t len)
{
   uint16_t index;
   for(index=0;index<len;index++)
			UNUSED_VARIABLE(app_uart_put(*data++));
}



static void uart_putbytes(char * fmt, int size)
{
		U_PutUARTBytes((uint8_t*)fmt, size);
		U_PutUARTBytes((uint8_t*)"\r\n", 2);
}

static void uart_timers_create(void)
{
    uint32_t err_code;

    err_code = app_timer_create(&uart_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                uart_timeout_handler);
    APP_ERROR_CHECK(err_code);
}

void uart_putbyte(const char *fmt, ...)
{
    static char logCbuf[1024];
    va_list args;
    char *p;
    int n, m;

    memset(logCbuf, 0, 1024);
    p = logCbuf;
    m = 1020;
    va_start(args, fmt);
    n = vsnprintf(p, m, fmt, args);
    va_end(args);

    uart_putbytes(logCbuf, n);
}



static void uart_init(void)
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

void uart_enable(bool status)
{
	if(true == status){
	 NRF_UART0->ENABLE        = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
   NRF_UART0->TASKS_STARTRX = 1;
   NRF_UART0->TASKS_STARTTX = 1;
	}else{
		NRF_UART0->TASKS_STOPTX = 1;
		NRF_UART0->TASKS_STOPRX = 1;
		NRF_UART0->ENABLE       = (UART_ENABLE_ENABLE_Disabled << UART_ENABLE_ENABLE_Pos);
	}
}


void init_uart(void)
{
	uart_init();
	uart_timers_create();
}

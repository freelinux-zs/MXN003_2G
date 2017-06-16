#ifndef __MODEM_2G_H
#define __MODEM_2G_H

#include "nrf.h"
#include "nrf_peripherals.h"
#include "nrf_assert.h"
#include <stdbool.h>
#include <stdlib.h>
#include "nrf_gpio.h"
#include "mxn003.h"

__STATIC_INLINE void modem_2g_open(uint32_t pin_number)
{
	nrf_gpio_pin_write(pin_number, 0);
}

__STATIC_INLINE void modem_2g_close(uint32_t pin_number)
{
	nrf_gpio_pin_write(pin_number, 1);
}

__STATIC_INLINE void modem_2g_init(uint32_t pin_number)
{
	nrf_gpio_cfg_output(pin_number);
}

#endif


#ifndef __MODEM_2G_H
#define __MODEM_2G_H

#include "nrf.h"
#include "nrf_peripherals.h"
#include "nrf_assert.h"
#include <stdbool.h>
#include <stdlib.h>
#include "nrf_gpio.h"
#include "mxn003.h"

#define  MODME_CONTRL_PIN MODEM_2G_PIN_NUMBER

__STATIC_INLINE void modem_2g_open(void)
{
	nrf_gpio_pin_write(MODME_CONTRL_PIN, 0);
}

__STATIC_INLINE void modem_2g_close(void)
{
	nrf_gpio_pin_write(MODME_CONTRL_PIN, 1);
}

__STATIC_INLINE void modem_2g_init(void)
{
	nrf_gpio_cfg_output(MODME_CONTRL_PIN);
	nrf_gpio_pin_write(MODME_CONTRL_PIN, 1);
}

#endif


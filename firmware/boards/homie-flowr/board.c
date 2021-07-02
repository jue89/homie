/*
 * Copyright (C) 2021 Juergen Fitschen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_homie-flowr
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Homie Flowr board
 *
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
#include "frequency_counter.h"
#endif

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
static const frequency_counter_params_t freq_params[] = {
    {
        .gpio_en = CH0_PWR_GPIO_PIN,
        .gpio_clk = CH0_SIG_GPIO_PIN,
        .name = "CH0_FREQ"
    },
    {
        .gpio_en = CH1_PWR_GPIO_PIN,
        .gpio_clk = CH1_SIG_GPIO_PIN,
        .name = "CH1_FREQ"
    },
    {
        .gpio_en = CH2_PWR_GPIO_PIN,
        .gpio_clk = CH2_SIG_GPIO_PIN,
        .name = "CH2_FREQ"
    },
    {
        .gpio_en = CH3_PWR_GPIO_PIN,
        .gpio_clk = CH3_SIG_GPIO_PIN,
        .name = "CH3_FREQ"
    },
};
static frequency_counter_t freq[ARRAY_SIZE(freq_params)];
#endif

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
    for (size_t i = 0; i < ARRAY_SIZE(freq_params); i++) {
        frequency_counter_add_device(&freq[i], &freq_params[i]);
    }
#endif
}

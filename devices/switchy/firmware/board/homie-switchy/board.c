/*
 * Copyright (C) 2021 Juergen Fitschen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_homie-switchy
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Homie Switchy board
 *
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include "hand_counter.h"

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
static const hand_counter_params_t sw_params[] = {
    {
        .gpio = SW0_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = 50000,
        .name = "BTN0"
    }
};
static hand_counter_t sw[ARRAY_SIZE(sw_params)];

#endif

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
    for (size_t i = 0; i < ARRAY_SIZE(sw_params); i++) {
        hand_counter_init(&sw[i], &sw_params[i]);
    }
#endif
}

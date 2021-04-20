/*
 * Copyright (C) 2021 Juergen Fitschen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_homie-pwmctrl
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Homie PWMCtrl board
 *
 * @author      Juergen Fitschen <me@jue.yt>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/pwm.h"
#include "dimmable_led.h"

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
static const dimmable_led_params_t led_params[] = {
    {
        .freq = 128,
        .name = "LEDRGB",
        .ch = {
            { .dev = PWM_DEV(0), .no = 0 },
            { .dev = PWM_DEV(0), .no = 1 },
            { .dev = PWM_DEV(0), .no = 2 }
        }
    },
    {
        .freq = 128,
        .name = "LEDW",
        .ch = {
            { .dev = PWM_DEV(0), .no = 3 },
            { .dev = PWM_UNDEF }
        }
    },
};

static dimmable_led_t led[ARRAY_SIZE(led_params)];
#endif

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
    for (size_t i = 0; i < ARRAY_SIZE(led_params); i++) {
        dimmable_led_init(&led[i], &led_params[i]);
    }
#endif
}

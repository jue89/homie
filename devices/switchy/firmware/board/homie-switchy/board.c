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
#include "dimmable_led.h"

#ifndef CONFIG_KCONFIG_BOARD_HOMIE_SWITCHY
#define CONFIG_BOARD_HOMIE_SWITCHY_SW0 1
#define CONFIG_BOARD_HOMIE_SWITCHY_SW1 1
#define CONFIG_BOARD_HOMIE_SWITCHY_SW2 1
#define CONFIG_BOARD_HOMIE_SWITCHY_SW3 1
#define CONFIG_BOARD_HOMIE_SWITCHY_SW4 1
#endif

#ifndef CONFIG_BOARD_HOMIE_SWITCHY_FREQ
#define CONFIG_BOARD_HOMIE_SWITCHY_FREQ 128
#endif

#ifndef CONFIG_BOARD_HOMIE_SWITCHY_DEBOUNCE
#define CONFIG_BOARD_HOMIE_SWITCHY_DEBOUNCE 50000
#endif

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
static const hand_counter_params_t sw_params[] = {
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW0)
    {
        .gpio = SW0_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_DEBOUNCE,
        .name = "BTN0"
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW1)
    {
        .gpio = SW1_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_DEBOUNCE,
        .name = "BTN1"
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW2)
    {
        .gpio = SW2_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_DEBOUNCE,
        .name = "BTN2"
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW3)
    {
        .gpio = SW3_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_DEBOUNCE,
        .name = "BTN3"
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW4)
    {
        .gpio = SW4_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_DEBOUNCE,
        .name = "BTN4"
    },
#endif
};
static hand_counter_t sw[ARRAY_SIZE(sw_params)];

static const dimmable_led_params_t led_params[] = {
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW0)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_FREQ,
        .name = "LED0",
        .ch = {
            { .dev = SW0_LED_PWM_DEV, .no = SW0_LED_PWM_CH },
            { .dev = PWM_UNDEF }
        }
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW1)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_FREQ,
        .name = "LED1",
        .ch = {
            { .dev = SW1_LED_PWM_DEV, .no = SW1_LED_PWM_CH },
            { .dev = PWM_UNDEF }
        }
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW2)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_FREQ,
        .name = "LED2",
        .ch = {
            { .dev = SW2_LED_PWM_DEV, .no = SW2_LED_PWM_CH },
            { .dev = PWM_UNDEF }
        }
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW3)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_FREQ,
        .name = "LED3",
        .ch = {
            { .dev = SW3_LED_PWM_DEV, .no = SW3_LED_PWM_CH },
            { .dev = PWM_UNDEF }
        }
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_SW4)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_FREQ,
        .name = "LED4",
        .ch = {
            { .dev = SW4_LED_PWM_DEV, .no = SW4_LED_PWM_CH },
            { .dev = PWM_UNDEF }
        }
    },
#endif
};
static dimmable_led_t led[ARRAY_SIZE(led_params)];
#endif

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
    for (size_t i = 0; i < ARRAY_SIZE(sw_params); i++) {
        hand_counter_init(&sw[i], &sw_params[i]);
    }

    for (size_t i = 0; i < ARRAY_SIZE(led_params); i++) {
        dimmable_led_init(&led[i], &led_params[i]);
    }
#endif
}

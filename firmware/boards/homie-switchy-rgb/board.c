/*
 * Copyright (C) 2021 Jue
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_homie-switchy-rgb
 * @{
 *
 * @file
 * @brief       Board specific implementations for the homie-switchy-rgb board
 *
 * @author      Jue <me@jue.yt>
 *
 * @}
 */

#include "cpu.h"
#include "board.h"
#include "periph/gpio.h"
#include <stdio.h>

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
#include "hand_counter.h"
#include "dimmable_led.h"
#endif

#ifndef CONFIG_KCONFIG_BOARD_HOMIE_SWITCHY_RGB
#define CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW0 1
#define CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW1 1
#define CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW2 1
#define CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW3 1
#define CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW4 1
#endif

#ifndef CONFIG_BOARD_HOMIE_SWITCHY_RGB_FREQ
#define CONFIG_BOARD_HOMIE_SWITCHY_RGB_FREQ 128
#endif

#ifndef CONFIG_BOARD_HOMIE_SWITCHY_RGB_DEBOUNCE
#define CONFIG_BOARD_HOMIE_SWITCHY_RGB_DEBOUNCE 50000
#endif

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
static const hand_counter_params_t sw_params[] = {
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW0)
    {
        .gpio = SW0_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_RGB_DEBOUNCE,
        .name = "BTN0"
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW1)
    {
        .gpio = SW1_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_RGB_DEBOUNCE,
        .name = "BTN1"
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW2)
    {
        .gpio = SW2_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_RGB_DEBOUNCE,
        .name = "BTN2"
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW3)
    {
        .gpio = SW3_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_RGB_DEBOUNCE,
        .name = "BTN3"
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW4)
    {
        .gpio = SW4_BTN_GPIO_PIN,
        .mode = GPIO_IN_PU,
        .debounce_usec = CONFIG_BOARD_HOMIE_SWITCHY_RGB_DEBOUNCE,
        .name = "BTN4"
    },
#endif
};
static hand_counter_t sw[ARRAY_SIZE(sw_params)];

static const dimmable_led_params_t led_params[] = {
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW0)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_RGB_FREQ,
        .invert = true,
        .name = "LED0",
        .ch = {
            { .dev = SW0_LED_R_PWM_DEV, .no = SW0_LED_R_PWM_CH },
            { .dev = SW0_LED_G_PWM_DEV, .no = SW0_LED_G_PWM_CH },
            { .dev = SW0_LED_B_PWM_DEV, .no = SW0_LED_B_PWM_CH }
        }
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW1)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_RGB_FREQ,
        .invert = true,
        .name = "LED1",
        .ch = {
            { .dev = SW1_LED_R_PWM_DEV, .no = SW1_LED_R_PWM_CH },
            { .dev = SW1_LED_G_PWM_DEV, .no = SW1_LED_G_PWM_CH },
            { .dev = SW1_LED_B_PWM_DEV, .no = SW1_LED_B_PWM_CH }
        }
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW2)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_RGB_FREQ,
        .invert = true,
        .name = "LED2",
        .ch = {
            { .dev = SW2_LED_R_PWM_DEV, .no = SW2_LED_R_PWM_CH },
            { .dev = SW2_LED_G_PWM_DEV, .no = SW2_LED_G_PWM_CH },
            { .dev = SW2_LED_B_PWM_DEV, .no = SW2_LED_B_PWM_CH }
        }
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW3)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_RGB_FREQ,
        .invert = true,
        .name = "LED3",
        .ch = {
            { .dev = SW3_LED_R_PWM_DEV, .no = SW3_LED_R_PWM_CH },
            { .dev = SW3_LED_G_PWM_DEV, .no = SW3_LED_G_PWM_CH },
            { .dev = SW3_LED_B_PWM_DEV, .no = SW3_LED_B_PWM_CH }
        }
    },
#endif
#if IS_ACTIVE(CONFIG_BOARD_HOMIE_SWITCHY_RGB_SW4)
    {
        .freq = CONFIG_BOARD_HOMIE_SWITCHY_RGB_FREQ,
        .invert = true,
        .name = "LED4",
        .ch = {
            { .dev = SW4_LED_R_PWM_DEV, .no = SW4_LED_R_PWM_CH },
            { .dev = SW4_LED_G_PWM_DEV, .no = SW4_LED_G_PWM_CH },
            { .dev = SW4_LED_B_PWM_DEV, .no = SW4_LED_B_PWM_CH }
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

    /* remap TIM2 and disable JTAG */
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    AFIO->MAPR = AFIO_MAPR_SWJ_CFG_JTAGDISABLE | AFIO_MAPR_TIM2_REMAP;

#if IS_ACTIVE(MODULE_SAUL_DEFAULT)
    for (size_t i = 0; i < ARRAY_SIZE(sw_params); i++) {
        hand_counter_init(&sw[i], &sw_params[i]);
    }

    for (size_t i = 0; i < ARRAY_SIZE(led_params); i++) {
        dimmable_led_init(&led[i], &led_params[i]);
    }
#endif
}

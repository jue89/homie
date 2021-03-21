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
 * @brief       Board specific definitions for the Homie Switchy
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STDIO_UART_DEV               UART_DEV(0)

#define CONFIG_ZTIMER_USEC_DEV       TIMER_DEV(0)
#define CONFIG_ZTIMER_USEC_WIDTH     (16)
#define CONFIG_ZTIMER_USEC_BASE_FREQ MHZ(1)

#define DOSE_PARAM_UART              UART_DEV(1)
#define DOSE_PARAM_SENSE_PIN         GPIO_PIN(PORT_A, 4)
//#define DOSE_PARAM_BAUDRATE          MHZ(1)

#define SW0_LED_PWM_DEV              PWM_DEV(0)
#define SW0_LED_PWM_CH               (1)
#define SW0_BTN_GPIO_PIN             GPIO_PIN(PORT_A, 12)

#define SW1_LED_PWM_DEV              PWM_DEV(0)
#define SW1_LED_PWM_CH               (0)
#define SW1_BTN_GPIO_PIN             GPIO_PIN(PORT_B, 15)

#define SW2_LED_PWM_DEV              PWM_DEV(1)
#define SW2_LED_PWM_CH               (2)
#define SW2_BTN_GPIO_PIN             GPIO_PIN(PORT_B, 2)

#define SW3_LED_PWM_DEV              PWM_DEV(1)
#define SW3_LED_PWM_CH               (1)
#define SW3_BTN_GPIO_PIN             GPIO_PIN(PORT_B, 6)

#define SW4_LED_PWM_DEV              PWM_DEV(1)
#define SW4_LED_PWM_CH               (0)
#define SW4_BTN_GPIO_PIN             GPIO_PIN(PORT_B, 3)

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

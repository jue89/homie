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
 * @brief       Board specific definitions for the Homie Flowr
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

#define CH0_PWR_GPIO_PIN             GPIO_PIN(PORT_B, 0)
#define CH0_LED_GPIO_PIN             GPIO_PIN(PORT_B, 2)
#define CH0_SIG_GPIO_PIN             GPIO_PIN(PORT_B, 1)

#define CH1_PWR_GPIO_PIN             GPIO_PIN(PORT_B, 12)
#define CH1_LED_GPIO_PIN             GPIO_PIN(PORT_B, 14)
#define CH1_SIG_GPIO_PIN             GPIO_PIN(PORT_B, 13)

#define CH2_PWR_GPIO_PIN             GPIO_PIN(PORT_B, 3)
#define CH2_LED_GPIO_PIN             GPIO_PIN(PORT_B, 6)
#define CH2_SIG_GPIO_PIN             GPIO_PIN(PORT_B, 5)

#define CH3_PWR_GPIO_PIN             GPIO_PIN(PORT_B, 7)
#define CH3_LED_GPIO_PIN             GPIO_PIN(PORT_B, 9)
#define CH3_SIG_GPIO_PIN             GPIO_PIN(PORT_B, 8)

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

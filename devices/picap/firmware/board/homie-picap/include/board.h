/*
 * Copyright (C) 2021 Juergen Fitschen
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_homie-picap
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Homie PiCap
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

#define STDIO_UART_DEV                UART_DEV(0)

#define CONFIG_ZTIMER_USEC_DEV        TIMER_DEV(0)
#define CONFIG_ZTIMER_USEC_WIDTH      (16)
#define CONFIG_ZTIMER_USEC_BASE_FREQ  MHZ(1)
#define CONFIG_ZTIMER_USEC_ADJUST_SET (15)

#define DOSE_PARAM_UART               UART_DEV(1)
#define DOSE_PARAM_SENSE_PIN          GPIO_PIN(PORT_A, 4)
//#define DOSE_PARAM_BAUDRATE          MHZ(1)

#define LED_TX_GPIO_PIN               GPIO_PIN(PORT_B, 2)
#define LED_RX_GPIO_PIN               GPIO_PIN(PORT_B, 1)

/**
 * @brief   Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */

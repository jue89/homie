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
 * @brief       Configuration of CPU peripherals for Homie Switchy board
 *
 * @author      Juergen Fitschen <me@jue.yt>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* This board has just a 12MHz crystal */
#define CONFIG_BOARD_HAS_LSE     0
#define CONFIG_BOARD_HAS_HSE     1
#define CLOCK_HSE                MHZ(12)

/* We want the board to run with a core clock of 48MHz */
#define CONFIG_CLOCK_PLL_PREDIV  (1)
#define CONFIG_CLOCK_PLL_MUL     (4)

#include "periph_cpu.h"
#include "clk_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   PWM configuration
 * @{
 */
static const pwm_conf_t pwm_config[] = {
    {
        .dev      = TIM1,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A, 8),  .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_A, 11), .cc_chan = 3 },
                      { .pin = GPIO_UNDEF } },
        .af       = GPIO_AF2,
        .bus      = APB2
    },
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 4), .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_B, 5), .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_B, 1), .cc_chan = 3 },
                      { .pin = GPIO_UNDEF } },
        .af       = GPIO_AF1,
        .bus      = APB1
    }
};

#define PWM_NUMOF ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
#define TIMER_0_MAX_VALUE   (0xffff)

static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM14,
        .max      = TIMER_0_MAX_VALUE,
        .rcc_mask = RCC_APB1ENR_TIM14EN,
        .bus      = APB1,
        .irqn     = TIM14_IRQn
    }
};

#define TIMER_0_ISR         (isr_tim14)
#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name   UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {
        .dev        = USART1,
        .rcc_mask   = RCC_APB2ENR_USART1EN,
        .rx_pin     = GPIO_PIN(PORT_A, 10),
        .tx_pin     = GPIO_PIN(PORT_A, 9),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB2,
        .irqn       = USART1_IRQn
    },
    {
        .dev        = USART2,
        .rcc_mask   = RCC_APB1ENR_USART2EN,
        .rx_pin     = GPIO_PIN(PORT_A, 3),
        .tx_pin     = GPIO_PIN(PORT_A, 2),
        .rx_af      = GPIO_AF1,
        .tx_af      = GPIO_AF1,
        .bus        = APB1,
        .irqn       = USART2_IRQn
    }
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_usart2)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */

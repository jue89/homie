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
 * @brief       Configuration of CPU peripherals for homie-switchy-rgb board
 *
 * @author      Jue <me@jue.yt>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

/* This board has just a 12MHz crystal */
#define CONFIG_BOARD_HAS_LSE     0
#define CONFIG_BOARD_HAS_HSE     1
#define CLOCK_HSE                MHZ(12)

#ifndef CONFIG_CLOCK_PLL_PREDIV
#define CONFIG_CLOCK_PLL_PREDIV  (1)
#endif

#ifndef CONFIG_CLOCK_PLL_MUL
#define CONFIG_CLOCK_PLL_MUL     (4)
#endif

/* We want the board to run with a core clock of 72MHz max */
#if CONFIG_CLOCK_PLL_MUL > 6
#undef CONFIG_CLOCK_PLL_MUL
#define CONFIG_CLOCK_PLL_MUL     (6)
#endif

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
        .dev      = TIM2,
        .rcc_mask = RCC_APB1ENR_TIM2EN,
        /* Remapping will happen during board init! */
        .chan     = { { .pin = GPIO_PIN(PORT_A, 15), .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_B, 3),  .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_B, 11), .cc_chan = 3 },
                      { .pin = GPIO_UNDEF } },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB1
    },
    {
        .dev      = TIM3,
        .rcc_mask = RCC_APB1ENR_TIM3EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A, 6),  .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_A, 7),  .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_B, 0),  .cc_chan = 2 },
                      { .pin = GPIO_UNDEF } },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB1
    },
    {
        .dev      = TIM4,
        .rcc_mask = RCC_APB1ENR_TIM4EN,
        .chan     = { { .pin = GPIO_PIN(PORT_B, 6),  .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_B, 7),  .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_B, 8),  .cc_chan = 2 },
                      { .pin = GPIO_UNDEF } },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB1
    },
    {
        .dev      = TIM5,
        .rcc_mask = RCC_APB1ENR_TIM5EN,
        .chan     = { { .pin = GPIO_PIN(PORT_A, 0),  .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_A, 1),  .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_A, 2),  .cc_chan = 2 },
                      { .pin = GPIO_UNDEF } },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB1
    },
    {
        .dev      = TIM8,
        .rcc_mask = RCC_APB2ENR_TIM8EN,
        .chan     = { { .pin = GPIO_PIN(PORT_C, 6),  .cc_chan = 0 },
                      { .pin = GPIO_PIN(PORT_C, 7),  .cc_chan = 1 },
                      { .pin = GPIO_PIN(PORT_C, 9),  .cc_chan = 3 },
                      { .pin = GPIO_UNDEF } },
        .af       = GPIO_AF_OUT_PP,
        .bus      = APB2
    },
};

#define PWM_NUMOF ARRAY_SIZE(pwm_config)
/** @} */

/**
 * @name   Timer configuration
 * @{
 */
static const timer_conf_t timer_config[] = {
    {
        .dev      = TIM1,
        .max      = 0x0000ffff,
        .rcc_mask = RCC_APB2ENR_TIM1EN,
        .bus      = APB2,
        .irqn     = TIM1_CC_IRQn
    },
};

#define TIMER_0_ISR         isr_tim1_cc

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
        .bus        = APB2,
        .irqn       = USART1_IRQn
    },
    {
        .dev        = UART4,
        .rcc_mask   = RCC_APB1ENR_UART4EN,
        .rx_pin     = GPIO_PIN(PORT_C, 11),
        .tx_pin     = GPIO_PIN(PORT_C, 10),
        .bus        = APB1,
        .irqn       = UART4_IRQn
    }
};

#define UART_0_ISR          (isr_usart1)
#define UART_1_ISR          (isr_uart4)

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */

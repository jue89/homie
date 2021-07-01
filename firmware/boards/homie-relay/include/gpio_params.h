#ifndef GPIO_PARAMS_H
#define GPIO_PARAMS_H

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    GPIO pin configuration
 */
static const  saul_gpio_params_t saul_gpio_params[] =
{
    {
        .name = "CH1",
        .pin = RELAY_CH1_PIN,
        .mode = GPIO_OUT,
        .flags = 0
    },
    {
        .name = "CH2",
        .pin = RELAY_CH2_PIN,
        .mode = GPIO_OUT,
        .flags = 0
    },
    {
        .name = "CH3",
        .pin = RELAY_CH3_PIN,
        .mode = GPIO_OUT,
        .flags = 0
    },
    {
        .name = "CH4",
        .pin = RELAY_CH4_PIN,
        .mode = GPIO_OUT,
        .flags = 0
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
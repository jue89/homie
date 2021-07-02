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
        .name = "CH0_LED",
        .pin = CH0_LED_GPIO_PIN,
        .mode = GPIO_OUT,
        .flags = 0
    },
    {
        .name = "CH1_LED",
        .pin = CH1_LED_GPIO_PIN,
        .mode = GPIO_OUT,
        .flags = 0
    },
    {
        .name = "CH2_LED",
        .pin = CH2_LED_GPIO_PIN,
        .mode = GPIO_OUT,
        .flags = 0
    },
    {
        .name = "CH3_LED",
        .pin = CH3_LED_GPIO_PIN,
        .mode = GPIO_OUT,
        .flags = 0
    },
};

#ifdef __cplusplus
}
#endif

#endif /* GPIO_PARAMS_H */
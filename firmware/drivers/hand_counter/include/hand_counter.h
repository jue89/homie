#ifndef HAND_COUNTER_H
#define HAND_COUNTER_H

#include "periph/gpio.h"
#include "saul_reg.h"
#include "ztimer.h"
#include "phydat.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    gpio_t gpio;
    gpio_mode_t mode;
    uint32_t debounce_usec;
    const char * name;
} hand_counter_params_t;

typedef struct {
    const hand_counter_params_t * params;
    phydat_t dat;
    int btn_value;
    ztimer_t btn_debounce;
    saul_reg_t saul_dev;
} hand_counter_t;

int hand_counter_init(hand_counter_t * counter, const hand_counter_params_t * params);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HAND_COUNTER_H */
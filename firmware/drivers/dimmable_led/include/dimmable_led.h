#ifndef DIMMABLE_LED_H
#define DIMMABLE_LED_H

#include "saul_reg.h"
#include "phydat.h"
#include "periph/pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    pwm_t dev;
    uint32_t freq;
    uint8_t ch;
    const char * name;
} dimmable_led_params_t;

typedef struct {
    const dimmable_led_params_t * params;
    phydat_t state;
    saul_reg_t saul_dev;
} dimmable_led_t;

int dimmable_led_init(dimmable_led_t * led, const dimmable_led_params_t * params);

#ifdef __cplusplus
}
#endif

#endif /* DIMMABLE_LED_H */
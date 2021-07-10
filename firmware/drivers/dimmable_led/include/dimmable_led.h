#ifndef DIMMABLE_LED_H
#define DIMMABLE_LED_H

#include "saul_reg.h"
#include "phydat.h"
#include "periph/pwm.h"
#include "ztimer.h"

#ifndef CONFIG_DIMMABLE_LED_STEP_COUNT
#define CONFIG_DIMMABLE_LED_STEP_COUNT 16
#endif

#ifndef CONFIG_DIMMABLE_LED_STEP_DURATION
#define CONFIG_DIMMABLE_LED_STEP_DURATION 40
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    pwm_t dev;
    uint8_t no;
} dimmable_led_ch_t;

typedef struct {
    uint32_t freq;
    const char * name;
    const bool invert;
    const dimmable_led_ch_t ch[PHYDAT_DIM];
} dimmable_led_params_t;

typedef struct {
    saul_reg_t saul_dev;
    const dimmable_led_params_t * params;
    uint8_t setpoint[PHYDAT_DIM];
    uint8_t val[PHYDAT_DIM];
    int16_t step[PHYDAT_DIM];
    ztimer_t timer;
} dimmable_led_t;

int dimmable_led_init(dimmable_led_t * led, const dimmable_led_params_t * params);

#ifdef __cplusplus
}
#endif

#endif /* DIMMABLE_LED_H */

#include <string.h>
#include "dimmable_led.h"

static const uint8_t _pwmtable[256] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255
};

static void _update_pwm(void *arg)
{
    dimmable_led_t *led = (dimmable_led_t *) arg;
    size_t dim;
    bool schedule_update = false;

    for (dim = 0; dim < PHYDAT_DIM && led->params->ch[dim].dev != PWM_UNDEF; dim++) {
        if (led->val[dim] > led->setpoint[dim]) {
            /* LED gets darker */
            if (led->val[dim] - led->setpoint[dim] < CONFIG_DIMMABLE_LED_STEP_SIZE) {
                led->val[dim] = led->setpoint[dim];
            } else {
                led->val[dim] -= CONFIG_DIMMABLE_LED_STEP_SIZE;
                schedule_update = true;
            }
        } else if (led->val[dim] < led->setpoint[dim]) {
            /* LED gets brigther */
            if (led->setpoint[dim] - led->val[dim] < CONFIG_DIMMABLE_LED_STEP_SIZE) {
                led->val[dim] = led->setpoint[dim];
            } else {
                led->val[dim] += CONFIG_DIMMABLE_LED_STEP_SIZE;
                schedule_update = true;
            }
        } else {
            /* LED reached its setpoint */
            continue;
        }

        pwm_set(led->params->ch[dim].dev, led->params->ch[dim].no, _pwmtable[led->val[dim]]);
    }

    if (schedule_update) {
        ztimer_set(ZTIMER_MSEC, &led->timer, CONFIG_DIMMABLE_LED_STEP_DURATION);
    }
}

static int _read(const void *arg, phydat_t *res)
{
    dimmable_led_t *led = (dimmable_led_t *) arg;
    size_t dim;
    unsigned state = irq_disable();
    res->unit = UNIT_NONE;
    res->scale = 0;
    for (dim = 0; dim < PHYDAT_DIM && led->params->ch[dim].dev != PWM_UNDEF; dim++) {
        res->val[dim] = led->setpoint[dim];
    }
    irq_restore(state);
    return dim;
}

static int _write(const void *arg, phydat_t *data)
{
    dimmable_led_t *led = (dimmable_led_t *) arg;
    size_t dim;
    ssize_t i;
    unsigned state = irq_disable();
    for (dim = 0; dim < PHYDAT_DIM && led->params->ch[dim].dev != PWM_UNDEF; dim++) {
        led->setpoint[dim] = (uint8_t) data->val[dim];
        for (i = 0; i < data->scale; i++) {
            led->setpoint[dim] *= 10;
        }
        for (i = 0; i > data->scale; i--) {
            led->setpoint[dim] /= 10;
        }
    }
    irq_restore(state);
    ztimer_remove(ZTIMER_MSEC, &led->timer);
    _update_pwm((void*) led);
    return dim;
}

static const saul_driver_t _saul_driver = {
    .read = _read,
    .write = _write,
    .type = SAUL_ACT_DIMMER
};

int dimmable_led_init(dimmable_led_t * led, const dimmable_led_params_t * params)
{
    led->params = params;

    /* setup timer */
    led->timer.callback = _update_pwm;
    led->timer.arg = (void*) led;

    /* turn on PWM hardware */
    for (size_t dim = 0; dim < PHYDAT_DIM && led->params->ch[dim].dev != PWM_UNDEF; dim++) {
        pwm_init(led->params->ch[dim].dev, PWM_LEFT, led->params->freq, _pwmtable[ARRAY_SIZE(_pwmtable) - 1]);
        pwm_poweron(led->params->ch[dim].dev);
    }

    /* setup saul device */
    led->saul_dev.dev = (void*) led;
    led->saul_dev.name = led->params->name;
    led->saul_dev.driver = &_saul_driver;

    /* Set PWM */
    _update_pwm((void*) led);

    return saul_reg_add(&led->saul_dev);
}

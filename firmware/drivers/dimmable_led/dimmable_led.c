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

static void _update_pwm(dimmable_led_t * led)
{
    while (led->state.scale > 0) {
        led->state.val[0] *= 10;
        led->state.scale -= 1;
    }

    while (led->state.scale < 0) {
        led->state.val[0] /= 10;
        led->state.scale += 1;
    }

    if (led->state.val[0] < 0) {
        led->state.val[0] = 0;
    }

    if (led->state.val[0] > (int16_t) ARRAY_SIZE(_pwmtable) - 1) {
        led->state.val[0] = (int16_t) ARRAY_SIZE(_pwmtable) - 1;
    }

    pwm_set(led->params->dev, led->params->ch, _pwmtable[led->state.val[0]]);
}

static int _read(const void *arg, phydat_t *res)
{
    dimmable_led_t *led = (dimmable_led_t *) arg;
    unsigned state = irq_disable();
    memcpy(res, &led->state, sizeof(phydat_t));
    irq_restore(state);
    return 1;
}

static int _write(const void *arg, phydat_t *data)
{
    dimmable_led_t *led = (dimmable_led_t *) arg;
    unsigned state = irq_disable();
    memcpy(&led->state, data, sizeof(phydat_t));
    _update_pwm(led);
    irq_restore(state);
    return 1;
}

static const saul_driver_t _saul_driver = {
    .read = _read,
    .write = _write,
    .type = SAUL_ACT_DIMMER
};

int dimmable_led_init(dimmable_led_t * led, const dimmable_led_params_t * params)
{
    led->params = params;

    /* turn on PWM hardware */
    pwm_init(led->params->dev, PWM_LEFT, led->params->freq, _pwmtable[ARRAY_SIZE(_pwmtable) - 1]);
    pwm_poweron(led->params->dev);
    _update_pwm(led);

    /* setup saul device */
    led->saul_dev.dev = (void *) led;
    led->saul_dev.name = led->params->name;
    led->saul_dev.driver = &_saul_driver;
    return saul_reg_add(&led->saul_dev);
}

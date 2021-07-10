#include <string.h>
#include "dimmable_led.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Generated using https://victornpb.github.io/gamma-table-generator/ with gamma of 2.0 */
static const uint16_t _pwmtable[256] = {
       0,     1,     4,     9,    16,    25,    36,    49,    65,    82,   101,   122,   145,   170,   198,   227,
     258,   291,   327,   364,   403,   444,   488,   533,   581,   630,   681,   735,   790,   848,   907,   969,
    1032,  1098,  1165,  1235,  1306,  1380,  1455,  1533,  1613,  1694,  1778,  1864,  1951,  2041,  2133,  2226,
    2322,  2420,  2520,  2621,  2725,  2831,  2939,  3049,  3161,  3274,  3390,  3508,  3628,  3750,  3874,  4000,
    4128,  4258,  4390,  4524,  4660,  4798,  4938,  5081,  5225,  5371,  5519,  5669,  5821,  5976,  6132,  6290,
    6450,  6612,  6777,  6943,  7111,  7282,  7454,  7628,  7805,  7983,  8164,  8346,  8530,  8717,  8905,  9096,
    9288,  9483,  9679,  9878, 10078, 10281, 10486, 10692, 10901, 11111, 11324, 11539, 11755, 11974, 12195, 12418,
   12642, 12869, 13098, 13329, 13562, 13796, 14033, 14272, 14513, 14756, 15001, 15248, 15497, 15748, 16001, 16256,
   16513, 16772, 17033, 17296, 17561, 17828, 18097, 18368, 18641, 18916, 19193, 19473, 19754, 20037, 20322, 20609,
   20899, 21190, 21483, 21778, 22076, 22375, 22676, 22980, 23285, 23593, 23902, 24213, 24527, 24842, 25160, 25479,
   25801, 26124, 26450, 26777, 27107, 27439, 27772, 28108, 28445, 28785, 29127, 29470, 29816, 30164, 30513, 30865,
   31219, 31575, 31933, 32292, 32654, 33018, 33384, 33752, 34122, 34493, 34867, 35243, 35621, 36001, 36383, 36767,
   37153, 37541, 37931, 38323, 38717, 39113, 39511, 39912, 40314, 40718, 41124, 41532, 41942, 42355, 42769, 43185,
   43603, 44024, 44446, 44870, 45297, 45725, 46155, 46588, 47022, 47458, 47897, 48337, 48780, 49224, 49671, 50119,
   50570, 51022, 51477, 51933, 52392, 52852, 53315, 53780, 54246, 54715, 55185, 55658, 56133, 56610, 57088, 57569,
   58052, 58537, 59023, 59512, 60003, 60496, 60991, 61488, 61986, 62487, 62990, 63495, 64002, 64511, 65022, 65535,
};

static unsigned intialized_pwm_devs = 0x00;

static void _update_pwm(void *arg)
{
    dimmable_led_t *led = (dimmable_led_t *) arg;
    size_t dim;
    bool schedule_update = false;

    for (dim = 0; dim < PHYDAT_DIM && led->params->ch[dim].dev != PWM_UNDEF; dim++) {
        if (led->step[dim] == 0) {
            /* we reached setpoint */
            continue;
        }

        int diff = led->setpoint[dim] - led->val[dim];
        if ((led->step[dim] < 0 && diff > led->step[dim]) || (led->step[dim] > 0 && diff < led->step[dim])) {
            /* this was the last iteration towards setpoint */
            led->val[dim] = led->setpoint[dim];
            led->step[dim] = 0;
        } else {
            /* further iterations are required */
            led->val[dim] += led->step[dim];
            schedule_update = true;
        }

        uint16_t value = _pwmtable[led->val[dim]];
        if (led->params->invert) {
            value = _pwmtable[ARRAY_SIZE(_pwmtable) - 1] - value;
        }

        pwm_set(led->params->ch[dim].dev, led->params->ch[dim].no, value);
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

static inline unsigned _div_ceil(unsigned a, unsigned b)
{
    return (a + b - 1) / b;
}

static int _write(const void *arg, phydat_t *data)
{
    dimmable_led_t *led = (dimmable_led_t *) arg;
    size_t dim;
    ssize_t i;
    unsigned state;

    /* abort ongoing fading */
    ztimer_remove(ZTIMER_MSEC, &led->timer);

    /* setup fading to setpoint */
    state = irq_disable();
    for (dim = 0; dim < PHYDAT_DIM && led->params->ch[dim].dev != PWM_UNDEF; dim++) {
        /* store the setpoint */
        led->setpoint[dim] = (uint8_t) data->val[dim];
        for (i = 0; i < data->scale; i++) {
            led->setpoint[dim] *= 10;
        }
        for (i = 0; i > data->scale; i--) {
            led->setpoint[dim] /= 10;
        }

        /* calc step width */
        if (led->setpoint[dim] >= led->val[dim]) {
            /* positive */
            led->step[dim] = _div_ceil(led->setpoint[dim] - led->val[dim], CONFIG_DIMMABLE_LED_STEP_COUNT);
        } else {
            /* negative */
            led->step[dim] = -1 * _div_ceil(led->val[dim] - led->setpoint[dim], CONFIG_DIMMABLE_LED_STEP_COUNT);
        }

        DEBUG("%d: Fade from %d to %d in steps of %d\n", dim, led->val[dim], led->setpoint[dim], led->step[dim]);
    }
    irq_restore(state);

    /* start fading process */
    _update_pwm((void*) led);

    return dim | SAUL_FLAG_QUEUE_EVENT;
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
        if ((intialized_pwm_devs & (1 << led->params->ch[dim].dev)) == 0) {
            pwm_init(led->params->ch[dim].dev, PWM_LEFT, led->params->freq, _pwmtable[ARRAY_SIZE(_pwmtable) - 1]);
            pwm_poweron(led->params->ch[dim].dev);
            intialized_pwm_devs |= (1 << led->params->ch[dim].dev);
        }

        uint16_t value = _pwmtable[led->val[dim]];
        if (led->params->invert) {
            value = _pwmtable[ARRAY_SIZE(_pwmtable) - 1] - value;
        }
        pwm_set(led->params->ch[dim].dev, led->params->ch[dim].no, value);
    }

    /* setup saul device */
    led->saul_dev.dev = (void*) led;
    led->saul_dev.name = led->params->name;
    led->saul_dev.driver = &_saul_driver;

    return saul_reg_add(&led->saul_dev);
}

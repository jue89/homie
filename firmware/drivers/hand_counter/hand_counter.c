#include <string.h>
#include "hand_counter.h"
#include "saul.h"
#include "saul_reg.h"
#if IS_ACTIVE(MODULE_SAUL_OBSERVER)
#include "saul_observer.h"
#endif

static int _read(const void *arg, phydat_t *res)
{
    hand_counter_t *counter = (hand_counter_t *) arg;
    memcpy(res, &counter->dat, sizeof(phydat_t));
    return 2;
}

static const saul_driver_t _saul_driver = {
    .read = _read,
    .write = saul_notsup,
    .type = SAUL_SENSE_COUNT
};

static void _atomic_increment(int16_t *cnt)
{
    unsigned state = irq_disable();
    if (*cnt == PHYDAT_MAX) {
        /* overflow int16 to 0 */
        *cnt = 0;
    } else {
        *cnt += 1;
    }
    irq_restore(state);
}

static void _timer_irq(void *arg)
{
    hand_counter_t *counter = (hand_counter_t *) arg;
    int btn_value;

    gpio_irq_enable(counter->params->gpio);

    /* read value after debounce time */
    btn_value = gpio_read(counter->params->gpio);
    if (btn_value == 0 && counter->btn_value > 0) {
        /* falling edge */
        _atomic_increment(&counter->dat.val[0]);
    } else if (btn_value > 0 && counter->btn_value == 0) {
        /* raising edge */
        _atomic_increment(&counter->dat.val[1]);
    } else {
        /* no flank detected ... */
        return;
    }

    counter->btn_value = btn_value;

#if IS_ACTIVE(MODULE_SAUL_OBSERVER)
    saul_observer_queue_event(&counter->saul_dev);
#endif
}

static void _gpio_irq(void *arg)
{
    hand_counter_t *counter = (hand_counter_t *) arg;

    /* wait defined time to debounce */
    gpio_irq_disable(counter->params->gpio);
    ztimer_set(ZTIMER_USEC, &counter->btn_debounce, counter->params->debounce_usec);
}

int hand_counter_init(hand_counter_t * counter, const hand_counter_params_t * params)
{
    counter->params = params;

    /* setup counter store */
    counter->dat.unit = UNIT_CTS ;
    counter->dat.scale = 0;

    /* setup gpio */
    gpio_init_int(counter->params->gpio, counter->params->mode, GPIO_BOTH, _gpio_irq, (void *) counter);
    counter->btn_value = gpio_read(counter->params->gpio);

    /* setup debounce timer */
    counter->btn_debounce.callback = _timer_irq;
    counter->btn_debounce.arg = (void *) counter;

    /* setup saul device */
    counter->saul_dev.dev = (void *) counter;
    counter->saul_dev.name = counter->params->name;
    counter->saul_dev.driver = &_saul_driver;
    return saul_reg_add(&counter->saul_dev);
}
#include "frequency_counter.h"
#include "ztimer.h"
#if IS_ACTIVE(MODULE_SAUL_OBSERVER)
#include "saul_observer.h"
#endif

static clist_node_t _counter_list;
static mutex_t _counter_list_mtx = MUTEX_INIT;
static char _stack[CONFIG_FREQUENCY_COUNTER_STACK_SIZE];

static inline uint32_t _abs_diff(uint32_t a, uint32_t b)
{
    return (a > b) ? a - b : b - a;
}

static void *_thread(void *arg)
{
    unsigned state;
    clist_node_t *next;
    frequency_counter_t *counter;
    uint32_t tmp;
    (void)arg;

    while (true) {
        ztimer_sleep(ZTIMER_MSEC, CONFIG_FREQUENCY_COUNTER_SLEEP_MSEC);

        /* Get first item in list */
        mutex_lock(&_counter_list_mtx);
        next = clist_lpeek(&_counter_list);
        mutex_unlock(&_counter_list_mtx);

        /* List is empty */
        if (next == NULL) {
            continue;
        }
        counter = container_of(next, frequency_counter_t, item);

        /* Enable device */
        if (counter->params->gpio_en != GPIO_UNDEF) {
            gpio_set(counter->params->gpio_en);
            ztimer_sleep(ZTIMER_MSEC, CONFIG_FREQUENCY_COUNTER_POWER_ON_DELAY_MSEC);
        }

        /* Measure */
        counter->cnt = 0;
        gpio_irq_enable(counter->params->gpio_clk);
        ztimer_sleep(ZTIMER_MSEC, 1000 / CONFIG_FREQUENCY_COUNTER_MEASURE_TIME_HZ);
        gpio_irq_disable(counter->params->gpio_clk);

        /* Disable device */
        if (counter->params->gpio_en != GPIO_UNDEF) {
            gpio_clear(counter->params->gpio_en);
        }

        /* Evaluate */
        state = irq_disable();
        tmp = counter->cnt * CONFIG_FREQUENCY_COUNTER_MEASURE_TIME_HZ;
        /* Ignore changes that might be caused by noisy bits */
        if (_abs_diff(tmp, counter->freq_cur) > CONFIG_FREQUENCY_COUNTER_MEASURE_TIME_HZ) {
            counter->freq_cur = tmp;
        }
#if IS_ACTIVE(MODULE_SAUL_OBSERVER)
        tmp = _abs_diff(counter->freq_cur, counter->freq_last_change);
        if (tmp > CONFIG_FREQUENCY_COUNTER_HYSTERESIS_HZ) {
            counter->freq_last_change = counter->freq_cur;
            saul_observer_queue_event(&counter->saul_dev);
        }
#endif
        irq_restore(state);

        /* Rotate list */
        mutex_lock(&_counter_list_mtx);
        clist_lpoprpush(&_counter_list);
        mutex_unlock(&_counter_list_mtx);
    }

    return NULL;
}

static void _gpio_irq(void *arg)
{
    frequency_counter_t * counter = arg;
    counter->cnt++;
}

static int _read(const void *arg, phydat_t *res)
{
    frequency_counter_t * counter = (frequency_counter_t *) arg;
    uint32_t val = counter->freq_cur;

    res->unit = UNIT_NONE;
    res->scale = 0;
    while (val > INT16_MAX) {
        val /= 10;
        res->scale += 1;
    }
    res->val[0] = (int16_t) val;

    return 1;
}

static const saul_driver_t _saul_driver = {
    .read = _read,
    .write = saul_notsup,
    .type = SAUL_SENSE_COUNT
};

int frequency_counter_add_device(frequency_counter_t * counter, const frequency_counter_params_t * params)
{
    counter->params = params;

    /* setup clk gpio */
    gpio_init_int(counter->params->gpio_clk, GPIO_IN_PD, GPIO_RISING, _gpio_irq, (void *) counter);
    gpio_irq_disable(counter->params->gpio_clk);

    /* setup enable gpio */
    if (counter->params->gpio_en != GPIO_UNDEF) {
        gpio_init(counter->params->gpio_en, GPIO_OUT);
        gpio_clear(counter->params->gpio_en);
    }

    /* Add to device list */
    mutex_lock(&_counter_list_mtx);
    clist_rpush(&_counter_list, &counter->item);
    mutex_unlock(&_counter_list_mtx);

    /* Add to SAUL registry */
    counter->saul_dev.dev = (void *) counter;
    counter->saul_dev.name = counter->params->name;
    counter->saul_dev.driver = &_saul_driver;
    return saul_reg_add(&counter->saul_dev);
}

void frequency_counter_init(void)
{
    thread_create(_stack, sizeof(_stack), CONFIG_FREQUENCY_COUNTER_PRIO,
                  THREAD_CREATE_STACKTEST,
                  _thread, NULL, "freq_cnt");
}

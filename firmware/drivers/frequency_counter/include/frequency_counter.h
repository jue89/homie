#ifndef FREQUENCY_COUNTER_H
#define FREQUENCY_COUNTER_H

#include "periph/gpio.h"
#include "saul_reg.h"
#include "clist.h"

#ifndef CONFIG_FREQUENCY_COUNTER_STACK_SIZE
#define CONFIG_FREQUENCY_COUNTER_STACK_SIZE 1024
#endif

#ifndef CONFIG_FREQUENCY_COUNTER_PRIO
#define CONFIG_FREQUENCY_COUNTER_PRIO 1
#endif

#ifndef CONFIG_FREQUENCY_COUNTER_SLEEP_MSEC
#define CONFIG_FREQUENCY_COUNTER_SLEEP_MSEC 100
#endif

#ifndef CONFIG_FREQUENCY_COUNTER_POWER_ON_DELAY_MSEC
#define CONFIG_FREQUENCY_COUNTER_POWER_ON_DELAY_MSEC 10
#endif
#ifndef CONFIG_FREQUENCY_COUNTER_MEASURE_TIME_HZ
#define CONFIG_FREQUENCY_COUNTER_MEASURE_TIME_HZ 10
#endif

#ifndef CONFIG_FREQUENCY_COUNTER_HYSTERESIS_HZ
#define CONFIG_FREQUENCY_COUNTER_HYSTERESIS_HZ 100
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    gpio_t gpio_en;
    gpio_t gpio_clk;
    const char * name;
} frequency_counter_params_t;

typedef struct {
    const frequency_counter_params_t * params;
    uint32_t cnt;
    uint32_t freq_cur;
#if IS_ACTIVE(MODULE_SAUL_OBSERVER)
    uint32_t freq_last_change;
#endif
    saul_reg_t saul_dev;
    clist_node_t item;
} frequency_counter_t;

int frequency_counter_add_device(frequency_counter_t * counter, const frequency_counter_params_t * params);

void frequency_counter_init(void);

#ifdef __cplusplus
}
#endif

#endif /* FREQUENCY_COUNTER_H */

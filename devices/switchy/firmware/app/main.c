#include <stdio.h>
#include "board.h"
#include "ztimer.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

static void btn_press (void* arg)
{
    (void) arg;
    puts("BTN");
}

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s MCU.\n", RIOT_MCU);

    gpio_init_int(SW0_BTN_GPIO_PIN, GPIO_IN_PU, GPIO_FALLING, btn_press, NULL);
    pwm_init(SW0_LED_PWM_DEV, PWM_LEFT, 256, 65535);
    pwm_poweron(SW0_LED_PWM_DEV);
    pwm_set(SW0_LED_PWM_DEV, SW0_LED_PWM_CH, 1200);

    while (1) {
        puts("Ping");
        ztimer_sleep(ZTIMER_USEC, 1000000);
    }

    return 0;
}

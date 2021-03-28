#include <stdio.h>
#include "net/netdev.h"
#include "net/gnrc/pktbuf.h"
#include "msg.h"
#include "thread.h"
#include "dose.h"
#include "dose_params.h"
#include "stdio_uart.h"
#include "ethos.h"
#include "ztimer.h"
#include "periph/gpio.h"
#include "periph/wdt.h"
#include "ps.h"
#include "fmt.h"

#define BRIDGE_MSG_TYPE_EVENT (0x4321)
#define BRIDGE_MSG_TYPE_PACKET (0x4322)
#define RCV_QUEUE_SIZE (32)
#define LED_TIME_USEC (50000)
#define WDT_TIME_MS (20000)

typedef struct {
    netdev_t * dev;
    kernel_pid_t pid;
    kernel_pid_t peer;
    char stack[THREAD_STACKSIZE_DEFAULT];
    gpio_t led;
    ztimer_t led_timer;
} bridge_port_t;

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    bridge_port_t *port = dev->context;

    if (event == NETDEV_EVENT_ISR) {
        /* Called from IRQ context -> send message to port thread */
        msg_t msg = { .type = BRIDGE_MSG_TYPE_EVENT, .content = { .ptr = dev } };
        if (msg_try_send(&msg, port->pid) <= 0) {
            puts("_event_cb: possibly lost interrupt.");
        }
    }
    else if (event == NETDEV_EVENT_RX_COMPLETE) {
        /* Called from port context */
        int bytes_expected;
        int bytes_read;
        gnrc_pktsnip_t *pkt;
        msg_t msg;

        /* Fetch byte count of the received packet */
        bytes_expected = dev->driver->recv(dev, NULL, 0, NULL);
        if (bytes_expected <= 0) return;

        /* Store received packet */
        pkt = gnrc_pktbuf_add(NULL, NULL, bytes_expected, GNRC_NETTYPE_UNDEF);
        if (!pkt) {
            puts("_event_cb: cannot allocate pktsnip.");
            /* drop the packet */
            dev->driver->recv(dev, NULL, bytes_expected, NULL);
            return;
        }

        bytes_read = dev->driver->recv(dev, pkt->data, bytes_expected, NULL);
        if (bytes_read <= 0) {
            gnrc_pktbuf_release(pkt);
            puts("_event_cb: empty packet.");
            return;
        }
        if (bytes_read < bytes_expected) {
            /* we've got less than the expected packet size,
             * so free the unused space.*/
            puts("_event_cb: reallocating.");
            printf("%d\n", bytes_read);
            gnrc_pktbuf_realloc_data(pkt, bytes_read);
        }

        msg.type = BRIDGE_MSG_TYPE_PACKET;
        msg.content.ptr = (void*) pkt;
        if (msg_try_send(&msg, port->peer) <= 0) {
            gnrc_pktbuf_release(pkt);
            puts("_event_cb: lost packet.");
        }
    }
    else if (event == NETDEV_EVENT_TX_COMPLETE) {
	    /* NOP */
    }
    else {
        printf("_event_cb: warning: unhandled event %u.\n", event);
    }
}

static void _gpio_clear(void *args)
{
    gpio_t * gpio = (gpio_t *) args;
    gpio_clear(*gpio);
}

static void *_port_thread(void *args)
{
    int rc;
    msg_t queue[RCV_QUEUE_SIZE];
    bridge_port_t *port = args;
    netopt_enable_t en = NETOPT_ENABLE;

    /* Prepare IPC */
    msg_init_queue(queue, RCV_QUEUE_SIZE);

    /* Setup netdev */
    port->dev->event_callback = _event_cb;
    rc = port->dev->driver->init(port->dev);
    port->dev->driver->set(port->dev, NETOPT_PROMISCUOUSMODE, &en, sizeof(en));
    if (rc < 0) {
        return NULL;
    }

    /* Setup LED */
    gpio_init(port->led, GPIO_OUT);
    port->led_timer.callback = _gpio_clear;
    port->led_timer.arg = &port->led;

    /* Store own PID */
    port->pid = thread_getpid();

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        wdt_kick();
        if (msg.type == BRIDGE_MSG_TYPE_EVENT) {
            /* Some event occured at the driver */
            port->dev->driver->isr(port->dev);
        }
        else if (msg.type == BRIDGE_MSG_TYPE_PACKET) {
            /* Got a message from peer port */
            ztimer_remove(ZTIMER_USEC, &port->led_timer);
            gpio_set(port->led);
            ztimer_set(ZTIMER_USEC, &port->led_timer, LED_TIME_USEC);

            gnrc_pktsnip_t *pkt = msg.content.ptr;
            port->dev->driver->send(port->dev, (iolist_t*) pkt);
            gnrc_pktbuf_release(pkt);
        }
    }

    return NULL;
}

void bridge_port_init (bridge_port_t *port, netdev_t *dev, const char * name, gpio_t led)
{
    int rc;

    port->dev = dev;
    port->pid = KERNEL_PID_UNDEF;
    port->peer = KERNEL_PID_UNDEF;
    port->led = led;
    dev->context = port;

    rc = thread_create(port->stack, sizeof(port->stack),
                       THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                       _port_thread, (void*) port, name);
    (void)rc;
    assert(rc > 0);
}

void bridge_port_connect (bridge_port_t *a, bridge_port_t *b)
{
    a->peer = b->pid;
    b->peer = a->pid;
}

static uint8_t ethos_inbuf[2048];
static const ethos_params_t ethos_params = {
    .uart = STDIO_UART_DEV,
    .baudrate = STDIO_UART_BAUDRATE,
    .buf = ethos_inbuf,
    .bufsize = sizeof(ethos_inbuf)
};

static dose_t dose;
static bridge_port_t dose_port;
ethos_t ethos;
static bridge_port_t ethos_port;

int main (void)
{
    puts("Start WDT");
    wdt_setup_reboot(0, WDT_TIME_MS);
    wdt_start();

    puts("Setup netifs");
    dose_setup(&dose, &dose_params[0], 0);
    ethos_setup(&ethos, &ethos_params);

    puts("Setup ports");
    bridge_port_init(&dose_port, (netdev_t*) &dose, "dose", LED_TX_GPIO_PIN);
    bridge_port_init(&ethos_port, (netdev_t*) &ethos, "ethos", LED_RX_GPIO_PIN);

    puts("Connect ports");
    bridge_port_connect(&dose_port, &ethos_port);

    while (1) {
        ztimer_sleep(ZTIMER_USEC, 10000000);
#if IS_ACTIVE(MODULE_PS)
	ps();
#endif
    }

    return 0;
}


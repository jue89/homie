#ifndef HDP_H
#define HDP_H

#include "net/gnrc/netif.h"
#include "net/gnrc/udp.h"

#ifndef CONFIG_HDP_MSG_QUEUE_SIZE
#define HDP_MSG_QUEUE_SIZE (16)
#endif

#ifndef HDP_THREAD_PRIO
#define HDP_THREAD_PRIO (GNRC_UDP_PRIO + 1)
#endif

#ifndef HDP_THREAD_STACK_SIZE
#define HDP_THREAD_STACK_SIZE (1024)
#endif

#ifndef CONFIG_HDP_STATE_PUBLISH_INTERVAL
#define CONFIG_HDP_STATE_PUBLISH_INTERVAL (10 * 1000)
#endif

#ifndef CONFIG_HDP_STARTUP_PUBLISH_DELAY
#define CONFIG_HDP_STARTUP_PUBLISH_DELAY (3 * 1000)
#endif

#ifndef CONFIG_HDP_PORT_CLIENT
#define CONFIG_HDP_PORT_CLIENT (5001)
#endif

#ifndef CONFIG_HDP_PORT_SERVER
#define CONFIG_HDP_PORT_SERVER (5000)
#endif

#ifndef CONFIG_HDP_MCAST_ADDR
#define CONFIG_HDP_MCAST_ADDR "ff02::cafe"
#endif

#define HDP_PARAMS_INIT {                   \
    .port_server = CONFIG_HDP_PORT_SERVER,  \
    .port_client = CONFIG_HDP_PORT_CLIENT,  \
    .multicast_addr = CONFIG_HDP_MCAST_ADDR,\
    .thread_name = "hdp",                   \
    .thread_prio = HDP_THREAD_PRIO          \
}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t port_server;
    uint16_t port_client;
    const char *multicast_addr;
    const char *thread_name;
    uint8_t thread_prio;
} hdp_params_t;

typedef struct {
    const hdp_params_t * params;
    char stack[HDP_THREAD_STACK_SIZE];
    gnrc_netif_t *netif;
    kernel_pid_t pid;
    unsigned saul_dev_count;
} hdp_ctx_t;

void hdp_init(hdp_ctx_t * ctx, const hdp_params_t * params);

#ifdef __cplusplus
}
#endif

#endif /* HDP_H */
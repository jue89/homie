#include "hdp.h"
#include "hdp_pkt.h"
#include "hdp_saul.h"
#include "thread.h"
#include "random.h"
#include "saul_observer.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/ipv6.h"
#include "nanocbor/nanocbor.h"

#define ENABLE_DEBUG 1
#include "debug.h"

#define MSG_TYPE_SEND_STATE 0xff01
#define MSG_TYPE_SEND_INFO_AND_STATE 0xff02
#define MSG_TYPE_SAUL_EVENT 0xff10

static int _send_udp (hdp_ctx_t *ctx, gnrc_pktsnip_t *pkt, const ipv6_addr_t *dst_addr, uint16_t dst_port)
{
    gnrc_pktsnip_t *udp, *ip, *netif;

    /* UDP */
    udp = gnrc_udp_hdr_build(pkt, ctx->params->port_server, dst_port);
    if (udp == NULL) {
        gnrc_pktbuf_release(pkt);
        return -ENOMEM;
    }

    /* IPv6 */
    ip = gnrc_ipv6_hdr_build(udp, NULL, dst_addr);
    if (ip == NULL) {
        gnrc_pktbuf_release(udp);
        return -ENOMEM;
    }

    /* Netif */
    netif = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    if (netif == NULL) {
        gnrc_pktbuf_release(ip);
        return -ENOMEM;
    }
    gnrc_netif_hdr_set_netif(netif->data, ctx->netif);
    ip = gnrc_pkt_prepend(ip, netif);

    /* Dispatch packet */
    if (!gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP, GNRC_NETREG_DEMUX_CTX_ALL, ip)) {
        gnrc_pktbuf_release(ip);
        return -EBADMSG;
    }

    return 0;
}

static void _publish_state(hdp_ctx_t *ctx, const ipv6_addr_t *dst_addr, uint16_t dst_port, int8_t *ids)
{
    gnrc_pktsnip_t *pkt;
    nanocbor_encoder_t cbor;

    hdp_pkt_enc_state_init(&pkt, &cbor);
    if (ids == NULL) {
        for (int i = 0; hdp_pkt_enc_state_add(&cbor, i) == 0; i++);
    } else {
        for (; *ids >= 0; ids++) {
            hdp_pkt_enc_state_add(&cbor, *ids);
        }
    }
    hdp_pkt_enc_state_finish(&pkt, &cbor);

    _send_udp(ctx, pkt, dst_addr, dst_port);
}

static void _publish_info(hdp_ctx_t *ctx, const ipv6_addr_t *dst_addr, uint16_t dst_port)
{
    gnrc_pktsnip_t *pkt;
    nanocbor_encoder_t cbor;

    hdp_pkt_enc_info_init(&pkt, &cbor);
    for (int i = 0; hdp_pkt_enc_info_add(&cbor, i) == 0; i++);
    hdp_pkt_enc_info_finish(&pkt, &cbor);

    _send_udp(ctx, pkt, dst_addr, dst_port);
}

static void _schedule_msg_timer(ztimer_t *timer, msg_t *msg, uint32_t base_offset)
{
    uint32_t offset = random_uint32_range(1 * base_offset, 2 * base_offset);
    ztimer_set_msg(ZTIMER_MSEC, timer, offset, msg, thread_getpid());
}

static void _subscribe_saul_observer(saul_observer_t *observer, size_t n, msg_t *observer_msg)
{
    size_t i;
    saul_reg_t *dev;
    for (i = 0, dev = saul_reg; i < n; i++, dev = dev->next) {
        saul_observer_msg(dev, &observer[i], observer_msg, thread_getpid());
    }
}

static int _saul_get_id(saul_reg_t *candidate)
{
    int i = 0;
    saul_reg_t *dev;
    for (i = 0, dev = saul_reg; dev != candidate; i++, dev = dev->next) {
        if (dev == NULL) {
            return -1;
        }
    }
    return i;
}

static void _get_sender(gnrc_pktsnip_t *pkt, ipv6_addr_t **addr, uint16_t *port)
{
    gnrc_pktsnip_t *udp, *ipv6;
    ipv6_hdr_t *ipv6_hdr;
    udp_hdr_t *udp_hdr;

    ipv6 = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_IPV6);
    ipv6_hdr = (ipv6_hdr_t *) ipv6->data;
    *addr = &ipv6_hdr->src;

    udp = gnrc_pktsnip_search_type(pkt, GNRC_NETTYPE_UDP);
    udp_hdr = (udp_hdr_t *) udp->data;
    *port = byteorder_ntohs (udp_hdr->src_port);
}

static void *_hdp_thread(void *arg)
{
    hdp_ctx_t * ctx = (hdp_ctx_t *) arg;
    msg_t msgq[HDP_MSG_QUEUE_SIZE];
    ipv6_addr_t multicast;
    saul_observer_t observer[ctx->saul_dev_count];
    msg_t observer_msg = { .type = MSG_TYPE_SAUL_EVENT };
    ztimer_t publish_timer;
    msg_t publish_msg = { .type = MSG_TYPE_SEND_INFO_AND_STATE };
    gnrc_netreg_entry_t netreg;

    /* setup multicast address */
    ipv6_addr_from_str(&multicast, ctx->params->multicast_addr);

    /* setup message queue */
    msg_init_queue(msgq, sizeof(msgq));

    /* setup SAUL observer */
    _subscribe_saul_observer(observer, ctx->saul_dev_count, &observer_msg);

    /* kick-off publish state timer */
    _schedule_msg_timer(&publish_timer, &publish_msg, CONFIG_HDP_STARTUP_PUBLISH_DELAY);

    /* bind in UDP port */
    gnrc_netreg_entry_init_pid(&netreg, ctx->params->port_server, thread_getpid());
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &netreg);

    while (1) {
        msg_t msg;
        msg_receive(&msg);
        if (msg.type == MSG_TYPE_SEND_STATE || msg.type == MSG_TYPE_SEND_INFO_AND_STATE) {
            if (msg.type == MSG_TYPE_SEND_INFO_AND_STATE) {
                _publish_info(ctx, &multicast, ctx->params->port_client);
                /* just publish the state next time */
                publish_msg.type = MSG_TYPE_SEND_STATE;
            }
            _publish_state(ctx, &multicast, ctx->params->port_client, NULL);
            _schedule_msg_timer(&publish_timer, &publish_msg, CONFIG_HDP_STATE_PUBLISH_INTERVAL);
        } else if (msg.type == MSG_TYPE_SAUL_EVENT) {
            int8_t ids[] = {-1, -1};
            int id = _saul_get_id(msg.content.ptr);
            if (id < 0) {
                continue;
            }
            ids[0] = id;
            _publish_state(ctx, &multicast, ctx->params->port_client, ids);
        } else if (msg.type == GNRC_NETAPI_MSG_TYPE_RCV) {
            gnrc_pktsnip_t *pkt = msg.content.ptr;
            nanocbor_value_t msg;
            nanocbor_decoder_init(&msg, pkt->data, pkt->size);
            int type = hdp_pkt_dec_type(&msg);
            if (type < 0) {
                DEBUG("DROP PACKET. REASON: %d\n", type);
            } else if (type == HDP_TAG_INFO_REQUEST) {
                ipv6_addr_t *dst_addr;
                uint16_t dst_port;
                _get_sender(pkt, &dst_addr, &dst_port);
                _publish_info(ctx, dst_addr, dst_port);
            } else if (type == HDP_TAG_STATE_REQUEST) {
                int8_t ids[ctx->saul_dev_count + 1];
                ipv6_addr_t *dst_addr;
                uint16_t dst_port;
                _get_sender(pkt, &dst_addr, &dst_port);
                if (hdp_pkt_dec_state_req(&msg, ids, sizeof(ids)) == 0) {
                    _publish_state(ctx, dst_addr, dst_port, ids);
                } else {
                    _publish_state(ctx, dst_addr, dst_port, NULL);
                }
            } else if (type == HDP_TAG_STATE_SET) {
                hdp_pkt_dec_state_set(&msg);
            } else {
                DEBUG("UNKNOWN TYPE: %d\n", type);
            }
            gnrc_pktbuf_release(pkt);
        }
    }

    return NULL;
}

void hdp_init(hdp_ctx_t * ctx, const hdp_params_t * params)
{
    ctx->params = params;

    /* count SAUL devices */
    saul_reg_t *dev;
    for (ctx->saul_dev_count = 0, dev = saul_reg; dev != NULL; ctx->saul_dev_count++, dev = dev->next);

    /* get first netif if no netif has been specified*/
    if (!ctx->netif) {
        ctx->netif = gnrc_netif_iter(NULL);
    }

    /* start thread */
    ctx->pid = thread_create(ctx->stack, sizeof(ctx->stack), ctx->params->thread_prio,
                             THREAD_CREATE_STACKTEST,
                             _hdp_thread, (void *) ctx, ctx->params->thread_name);
}
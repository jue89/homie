#include "hdp_pkt.h"
#include "hdp_saul.h"
#include "saul.h"
#include "periph/cpuid.h"
#include "ztimer.h"

int hdp_pkt_enc_info_init(gnrc_pktsnip_t **pkt, nanocbor_encoder_t *cbor)
{
    uint8_t cpuid[CPUID_LEN];

    *pkt = gnrc_pktbuf_add(NULL, NULL, HDP_MAX_PKT_SIZE, GNRC_NETTYPE_UNDEF);
    if (*pkt == NULL) {
        return -1;
    }

    nanocbor_encoder_init(cbor, (*pkt)->data, (*pkt)->size);

    /* build packet */
    nanocbor_fmt_tag(cbor, HDP_TAG_INFO_PUBLISH);
    nanocbor_fmt_map(cbor, 3);
    /* 0 - cpuid */
    nanocbor_fmt_int(cbor, 0);
    cpuid_get(cpuid);
    nanocbor_put_bstr(cbor, cpuid, sizeof(cpuid));
    /* 1 - uptime */
    nanocbor_fmt_int(cbor, 1);
    nanocbor_fmt_uint(cbor, ztimer_now(ZTIMER_MSEC));
    /* 2 - info map */
    nanocbor_fmt_int(cbor, 2);
    nanocbor_fmt_map_indefinite(cbor);

    return 0;
}

int hdp_pkt_enc_info_add(nanocbor_encoder_t *cbor, int id)
{
    saul_reg_t * item = saul_reg_find_nth(id);
    if (item == NULL) {
        return -1;
    }

    /* add item to map */
    nanocbor_fmt_int(cbor, id);
    hdp_cbor_info_saul(cbor, item);

    return 0;
}

int hdp_pkt_enc_info_finish(gnrc_pktsnip_t **pkt, nanocbor_encoder_t *cbor)
{
    /* close map */
    nanocbor_fmt_end_indefinite(cbor);

    /* check length */
    if (nanocbor_encoded_len(cbor) > (*pkt)->size) {
        gnrc_pktbuf_release(*pkt);
        return -1;
    }

    /* resize pktsnip */
    gnrc_pktbuf_realloc_data(*pkt, nanocbor_encoded_len(cbor));

    return 0;
}

int hdp_pkt_enc_state_init(gnrc_pktsnip_t **pkt, nanocbor_encoder_t *cbor)
{
    uint8_t cpuid[CPUID_LEN];

    *pkt = gnrc_pktbuf_add(NULL, NULL, HDP_MAX_PKT_SIZE, GNRC_NETTYPE_UNDEF);
    if (*pkt == NULL) {
        return -1;
    }

    nanocbor_encoder_init(cbor, (*pkt)->data, (*pkt)->size);

    /* build packet */
    nanocbor_fmt_tag(cbor, HDP_TAG_INFO_PUBLISH);
    nanocbor_fmt_map(cbor, 3);
    /* 0 - cpuid */
    nanocbor_fmt_int(cbor, 0);
    cpuid_get(cpuid);
    nanocbor_put_bstr(cbor, cpuid, sizeof(cpuid));
    /* 1 - uptime */
    nanocbor_fmt_int(cbor, 1);
    nanocbor_fmt_uint(cbor, ztimer_now(ZTIMER_MSEC));
    /* 2 - state map */
    nanocbor_fmt_int(cbor, 2);
    nanocbor_fmt_map_indefinite(cbor);

    return 0;
}

int hdp_pkt_enc_state_add(nanocbor_encoder_t *cbor, int id)
{
    saul_reg_t * item = saul_reg_find_nth(id);
    if (item == NULL) {
        return -1;
    }

    /* add item to map if it's readable */
    if (item->driver->read != saul_notsup) {
        nanocbor_fmt_int(cbor, id);
        hdp_cbor_read_saul(cbor, item);
    }

    return 0;
}

int hdp_pkt_enc_state_finish(gnrc_pktsnip_t **pkt, nanocbor_encoder_t *cbor)
{
    /* close map */
    nanocbor_fmt_end_indefinite(cbor);

    /* check length */
    if (nanocbor_encoded_len(cbor) > (*pkt)->size) {
        gnrc_pktbuf_release(*pkt);
        return -1;
    }

    /* resize pktsnip */
    gnrc_pktbuf_realloc_data(*pkt, nanocbor_encoded_len(cbor));

    return 0;
}

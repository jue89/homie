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
    nanocbor_fmt_tag(cbor, HDP_TAG_STATE_PUBLISH);
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

int _get_msg_item(nanocbor_value_t *cbor, nanocbor_value_t *ptr, int8_t item_key)
{
    int rc;

    /* enter the map */
    rc = nanocbor_enter_map(cbor, ptr);
    if (rc < 0) {
        return -1;
    }

    /* search for key */
    while (!nanocbor_at_end(ptr)) {
        int8_t key;
        rc = nanocbor_get_int8(ptr, &key);
        if (rc < 0) {
            /* Key wasn't a int8_t ... skip key and value */
            nanocbor_skip(ptr);
            nanocbor_skip(ptr);
            continue;
        }
        if (key == item_key) {
            /* We found it !*/
            return NANOCBOR_OK;
        } else {
            /* Wrong key ... skip value*/
            nanocbor_skip(ptr);
        }
    }

    return -1;
}

int hdp_pkt_dec_type(nanocbor_value_t *cbor)
{
    uint8_t cpuid[CPUID_LEN];
    int rc;
    uint32_t tag;
    nanocbor_value_t map;

    /* read tag */
    rc = nanocbor_get_tag(cbor, &tag);
    if (rc < 0) {
        return -1;
    }
    if (tag != HDP_TAG_INFO_REQUEST && tag != HDP_TAG_STATE_REQUEST && tag != HDP_TAG_STATE_SET) {
        return -2;
    }

    /* look for cpuid */
    rc = _get_msg_item(cbor, &map, 0);
    if (rc == NANOCBOR_OK) {
        const uint8_t * value;
        size_t value_len;
        rc = nanocbor_get_bstr(&map, &value, &value_len);
        if (rc < 0) {
            return -3;
        }
        if (value_len != sizeof(cpuid)) {
            return -3;
        }
        cpuid_get(cpuid);
        if (memcmp(cpuid, value, sizeof(cpuid)) != 0) {
            return -3;
        }
    }

    /* cpuid matches or not cpuid has been stated */
    return (int) tag;
}

int hdp_pkt_dec_state_set(nanocbor_value_t *msg)
{
    nanocbor_value_t item, map;
    int rc;

    /* open map with states */
    rc = _get_msg_item(msg, &item, 2);
    if (rc < 0) {
        return rc;
    }

    rc = nanocbor_enter_map(&item, &map);
    if (rc < 0) {
        return rc;
    }

    while (!nanocbor_at_end(&map)) {
        uint8_t id;
        rc = nanocbor_get_uint8(&map, &id);
        if (rc < 0) {
            /* Skip invalid entries*/
            nanocbor_skip(&map);
        } else {
            saul_reg_t * dev = saul_reg_find_nth(id);
            if (dev == NULL) {
                /* Invalid SAUL device */
                continue;
            }
            hdp_cbor_write_saul(dev, &map);
        }
    }

    return 0;
}
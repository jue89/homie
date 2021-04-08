#ifndef HDP_PKT_H
#define HDP_PKT_H

#include "nanocbor/nanocbor.h"
#include "net/gnrc/pktbuf.h"

#define HDP_MAX_PKT_SIZE (1000)
#define HDP_TAG_INFO_REQUEST (55)
#define HDP_TAG_INFO_PUBLISH (56)
#define HDP_TAG_STATE_REQUEST (57)
#define HDP_TAG_STATE_PUBLISH (58)
#define HDP_TAG_STATE_SET (58)

#ifdef __cplusplus
extern "C" {
#endif

int hdp_pkt_enc_info_init(gnrc_pktsnip_t **pkt, nanocbor_encoder_t *cbor);
int hdp_pkt_enc_info_add(nanocbor_encoder_t *cbor, int id);
int hdp_pkt_enc_info_finish(gnrc_pktsnip_t **pkt, nanocbor_encoder_t *cbor);

int hdp_pkt_enc_state_init(gnrc_pktsnip_t **pkt, nanocbor_encoder_t *cbor);
int hdp_pkt_enc_state_add(nanocbor_encoder_t *cbor, int id);
int hdp_pkt_enc_state_finish(gnrc_pktsnip_t **pkt, nanocbor_encoder_t *cbor);

int hdp_pkt_dec_type(nanocbor_value_t *cbor);
int hdp_pkt_dec_state_set(nanocbor_value_t *cbor);

#ifdef __cplusplus
}
#endif

#endif /* HDP_PKT_H */
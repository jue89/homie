#ifndef HDP_SAUL_H
#define HDP_SAUL_H

#include "nanocbor/nanocbor.h"
#include "saul_reg.h"

#ifdef __cplusplus
extern "C" {
#endif

int hdp_cbor_read_saul(nanocbor_encoder_t *cbor, saul_reg_t *dev);

int hdp_cbor_write_saul(saul_reg_t *dev, nanocbor_value_t *cbor);

int hdp_cbor_info_saul(nanocbor_encoder_t *cbor, saul_reg_t *dev);


#ifdef __cplusplus
}
#endif

#endif /* HDP_SAUL_H */
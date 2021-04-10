#include "hdp_saul.h"
#include "saul.h"

int hdp_cbor_read_saul(nanocbor_encoder_t *cbor, saul_reg_t *dev)
{
    phydat_t dat;
    size_t i;
    int dim;

    /* read from saul device */
    dim = saul_reg_read(dev, &dat);
    if (dim <= 0) {
        return dim;
    }

    /* encode cbor */
    nanocbor_fmt_array(cbor, dim + 1);
    nanocbor_fmt_int(cbor, dat.scale);
    for (i = 0; i < (size_t) dim; i++) {
        nanocbor_fmt_int(cbor, dat.val[i]);
    }

    return 0;
}

int hdp_cbor_write_saul(saul_reg_t *dev, nanocbor_value_t *cbor)
{
    nanocbor_value_t array;
    phydat_t dat;
    size_t i;
    int rc;

    /* read from cbor array */
    rc = nanocbor_enter_array(cbor, &array);
    if (rc < 0) {
        return rc;
    }
    rc = nanocbor_get_int8(&array, &dat.scale);
    if (rc < 0) {
        goto exit;
    }
    for (i = 0; i < PHYDAT_DIM; i++) {
        if (nanocbor_at_end(&array)) {
            dat.val[i] = 0;
        } else {
            rc = nanocbor_get_int16(&array, &dat.val[i]);
            if (rc < 0) {
                goto exit;
            }
        }
    }

    /* write to SAUL device */
    rc = saul_reg_write(dev, &dat);

exit:
    nanocbor_leave_container(cbor, &array);
    return rc;
}

int hdp_cbor_info_saul(nanocbor_encoder_t *cbor, saul_reg_t *dev)
{
    nanocbor_fmt_array(cbor, 4);
    if (!dev->name) {
        nanocbor_fmt_null(cbor);
    } else {
        nanocbor_put_tstr(cbor, dev->name);
    }
    nanocbor_put_tstr(cbor, saul_class_to_str(dev->driver->type));
    nanocbor_fmt_bool(cbor, dev->driver->read != saul_notsup);
    nanocbor_fmt_bool(cbor, dev->driver->write != saul_notsup);

    return 0;
}

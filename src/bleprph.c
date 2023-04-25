#include <bleprph.h>

static const char *manuf_name = "Tempest";
static const char *model_num = "PROTO-0";

char *device_name = "Tempest";
uint8_t blehr_addr_type;
uint16_t target_temp_val_handle;
uint16_t temp_val_handle;
uint16_t rh_val_handle;
uint16_t conn_handle;
bool notify_state;

static int gatt_svc_access_envs(
    uint16_t conn_handle, 
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg
);

static int gatt_svc_access_autoio(
    uint16_t conn_handle, 
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg
);

static int gatt_svc_access_device_info(
    uint16_t conn_handle, 
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg
);

static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /*** Service ***/
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_SVC_ENVS_UUID),
        .characteristics = (struct ble_gatt_chr_def[])
        { 
            {
                .uuid = BLE_UUID16_DECLARE(GATT_CHR_TEMP_UUID),
                .access_cb = gatt_svc_access_envs,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &temp_val_handle
            }, 
            {
                .uuid = BLE_UUID16_DECLARE(GATT_CHR_RH_UUID),
                .access_cb = gatt_svc_access_envs,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &rh_val_handle
            },
            {
                0, /* No more characteristics in this service. */
            }
        },
    },
    {
        /*** Service ***/
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_SVC_AUTOIO_UUID),
        .characteristics = (struct ble_gatt_chr_def[])
        { 
            {
                .uuid = BLE_UUID16_DECLARE(GATT_CHR_TEMP_UUID),
                .access_cb = gatt_svc_access_autoio,
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
                .val_handle = &target_temp_val_handle
            },
            {
                0, /* No more characteristics in this service. */
            }
        },
    },
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(GATT_DEVICE_INFO_UUID),
        .characteristics = (struct ble_gatt_chr_def[])
        { 
            {
                /* Characteristic: * Manufacturer name */
                .uuid = BLE_UUID16_DECLARE(GATT_MANUFACTURER_NAME_UUID),
                .access_cb = gatt_svc_access_device_info,
                .flags = BLE_GATT_CHR_F_READ,
            }, 
            {
                /* Characteristic: Model number string */
                .uuid = BLE_UUID16_DECLARE(GATT_MODEL_NUMBER_UUID),
                .access_cb = gatt_svc_access_device_info,
                .flags = BLE_GATT_CHR_F_READ,
            }, 
            {
                0, /* No more characteristics in this service */
            },
        }
    },
    {
        0, /* No more services. */
    },
};

static int gatt_svc_access_envs(
    uint16_t conn_handle, 
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg
) {
    uint16_t uuid;
    int ret;

    uuid = ble_uuid_u16(ctxt->chr->uuid);
    if(uuid == GATT_CHR_TEMP_UUID) {
        int16_t conv_temp = (int16_t)(curr_temp / (1 * pow(10 ,-2) * pow(2, 0)));
        ret = os_mbuf_append(ctxt->om, &conv_temp, sizeof(conv_temp));
        return ret == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    if(uuid == GATT_CHR_RH_UUID) {
        uint16_t conv_rh = (uint16_t)(curr_rh / (1 * pow(10, -2) * pow(2, 0)));
        ret = os_mbuf_append(ctxt->om, &conv_rh, sizeof(conv_rh));
        return ret == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int gatt_svc_access_autoio(
    uint16_t conn_handle, 
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg
) {
    uint16_t uuid;
    int ret;

    uuid = ble_uuid_u16(ctxt->chr->uuid);
    if(uuid == GATT_CHR_TEMP_UUID) {
        switch (ctxt->op) {
            case BLE_GATT_ACCESS_OP_READ_CHR:
                int16_t conv_temp = (int16_t)(target_temp / (1 * pow(10 ,-2) * pow(2, 0)));
                ret = os_mbuf_append(ctxt->om, &conv_temp, sizeof(conv_temp));
                return ret == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
                goto unknown;
            case BLE_GATT_ACCESS_OP_WRITE_CHR:
                int16_t raw_target_temp;
                ret = gatt_svr_write(
                    ctxt->om, 
                    sizeof(raw_target_temp),
                    sizeof(raw_target_temp),
                    &raw_target_temp, NULL);
                target_temp = (float)(raw_target_temp * (1 * pow(10, -2) * pow(2, 0)));
                ble_gatts_chr_updated(attr_handle);
                return ret == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
                goto unknown;
            default:
                assert(0);
                return BLE_ATT_ERR_UNLIKELY;
        }
    }
    unknown:
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

static int gatt_svc_access_device_info(
    uint16_t conn_handle, 
    uint16_t attr_handle,
    struct ble_gatt_access_ctxt *ctxt,
    void *arg
) {
    uint16_t uuid;
    int ret;

    uuid = ble_uuid_u16(ctxt->chr->uuid);
    if (uuid == GATT_MODEL_NUMBER_UUID) {
        ret = os_mbuf_append(ctxt->om, model_num, strlen(model_num));
        return ret == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    if (uuid == GATT_MANUFACTURER_NAME_UUID) {
        ret = os_mbuf_append(ctxt->om, manuf_name, strlen(manuf_name));
        return ret == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }
    assert(0);
    return BLE_ATT_ERR_UNLIKELY;
}

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    char buf[BLE_UUID_STR_LEN];

    switch (ctxt->op) {
        case BLE_GATT_REGISTER_OP_SVC:
            ESP_LOGD(
                "BLE_SVR_REG", "registered service %s with handle=%d\n",
                ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                ctxt->svc.handle
            );
            break;

        case BLE_GATT_REGISTER_OP_CHR:
            ESP_LOGD(
                "BLE_SVR_REG", "registering characteristic %s with "
                "def_handle=%d val_handle=%d\n",
                ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                ctxt->chr.def_handle,
                ctxt->chr.val_handle
            );
            break;

        case BLE_GATT_REGISTER_OP_DSC:
            ESP_LOGD(
                "BLE_SVR_REG", "registering descriptor %s with handle=%d\n",
                ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                ctxt->dsc.handle
            );
            break;

        default:
            assert(0);
            break;
    }
}

int gatt_svr_init(void)
{
    int ret;
    ble_svc_gap_init();
    ble_svc_gatt_init();

    ret = ble_gatts_count_cfg(gatt_svr_svcs);
    if (ret != 0) {
        return ret;
    }

    ret = ble_gatts_add_svcs(gatt_svr_svcs);
    if (ret != 0) {
        return ret;
    }

    return 0;
}

int gatt_svr_write(struct os_mbuf *om, uint16_t min_len, uint16_t max_len, void *dst, uint16_t *len)
{
    uint16_t om_len;
    int rc;

    om_len = OS_MBUF_PKTLEN(om);
    if (om_len < min_len || om_len > max_len) {
        return BLE_ATT_ERR_INVALID_ATTR_VALUE_LEN;
    }

    rc = ble_hs_mbuf_to_flat(om, dst, max_len, len);
    if (rc != 0) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    return 0;
}

int ble_gap_event_func(struct ble_gap_event *event, void *arg) {
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            /* A new connection was established or a connection attempt failed */
            ESP_LOGD(
                "BLE_GAP_EVT", "connection %s; status=%d\n",
                event->connect.status == 0 ? "established" : "failed",
                event->connect.status
            );
            if (event->connect.status != 0) {
                /* Connection failed; resume advertising */
                ble_advertise();
            }
            conn_handle = event->connect.conn_handle;
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            ESP_LOGD(
                "BLE_GAP_EVT", "disconnect; reason=%d\n", 
                event->disconnect.reason
            );
            /* Connection terminated; resume advertising */
            ble_advertise();
            break;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            ESP_LOGD(
                "BLE_GAP_EVT", "adv complete\n"
            );
            ble_advertise();
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
            ESP_LOGD(
                "BLE_GAP_EVT", "subscribe event; cur_notify=%d\n value handle; val_handle=%d\n",
                event->subscribe.cur_notify, 
                event->subscribe.attr_handle
            );
            notify_state = event->subscribe.cur_notify;
            ESP_LOGD("BLE_GAP_SUBSCRIBE_EVENT", "conn_handle from subscribe=%d", conn_handle);
            break;

        case BLE_GAP_EVENT_MTU:
            ESP_LOGD(
                "BLE_GAP_EVT", "mtu update event; conn_handle=%d mtu=%d\n",
                event->mtu.conn_handle,
                event->mtu.value
            );
            break;
    }

    return 0;
}

void ble_advertise(void) {
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    /*
     *  Set the advertisement data included in our advertisements:
     *     o Flags (indicates advertisement type and other general info)
     *     o Advertising tx power
     *     o Device name
     */
    memset(&fields, 0, sizeof(fields));

    /*
     * Advertise two flags:
     *      o Discoverability in forthcoming advertisement (general)
     *      o BLE-only (BR/EDR unsupported)
     */
    fields.flags = BLE_HS_ADV_F_DISC_GEN |
                   BLE_HS_ADV_F_BREDR_UNSUP;

    /*
     * Indicate that the TX power level field should be included; have the
     * stack fill this value automatically.  This is done by assigning the
     * special value BLE_HS_ADV_TX_PWR_LVL_AUTO.
     */
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;

    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        ESP_LOGE("BLE_EVT", "error setting advertisement data; rc=%d\n", rc);
        return;
    }

    /* Begin advertising */
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    rc = ble_gap_adv_start(blehr_addr_type, NULL, BLE_HS_FOREVER,
                           &adv_params, ble_gap_event_func, NULL);
    if (rc != 0) {
        ESP_LOGE("BLE_EVT", "error enabling advertisement; rc=%d\n", rc);
        return;
    }
}
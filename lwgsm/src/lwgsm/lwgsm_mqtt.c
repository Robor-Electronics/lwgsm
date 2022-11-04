#include "lwgsm/lwgsm_mqtt.h"
#include "lwgsm/lwgsm_private.h"

lwgsmr_t lwgsm_mqtt_pub(
        const char *address,
        const char *user,
        const char *topic,
        const char *client_id,
        const char *data,
        size_t length
) {
    LWGSM_MSG_VAR_DEFINE(msg);
    LWGSM_MSG_VAR_ALLOC(msg, 1);
    LWGSM_MSG_VAR_SET_EVT(msg, NULL, NULL);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_MQTT_PUB;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.address = address;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.mqtt.user = user;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.mqtt.topic = topic;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.mqtt.client_id = client_id;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.data = data;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.length = length;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 30000);
}

lwgsmr_t lwgsm_http_post(
        const char *address,
        const char *data,
        size_t length
) {
    LWGSM_MSG_VAR_DEFINE(msg);
    LWGSM_MSG_VAR_ALLOC(msg, 1);
    LWGSM_MSG_VAR_SET_EVT(msg, NULL, NULL);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_HTTP_POST;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.http.content_type = CONTENT_TYPE_JSON;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.address = address;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.data = data;
    LWGSM_MSG_VAR_REF(msg).msg.service_call.length = length;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 30000);
}

lwgsmr_t lwgsm_request_mobile_subscriber_id(
        char *subscribe_id,
        size_t length,
        lwgsm_api_cmd_evt_fn evt_fn,
        void *evt_arg,
        uint32_t blocking
) {
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_ASSERT(subscribe_id != NULL);
    LWGSM_ASSERT(length > 0);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_CIMI;
    LWGSM_MSG_VAR_REF(msg).cmd = LWGSM_CMD_CIMI;
    LWGSM_MSG_VAR_REF(msg).msg.device_info.str = subscribe_id;
    LWGSM_MSG_VAR_REF(msg).msg.device_info.len = length;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 10000);
}

lwgsmr_t lwgsm_cancel_call() {
    LWGSM_MSG_VAR_DEFINE(msg);
    LWGSM_MSG_VAR_ALLOC(msg, 1);
    LWGSM_MSG_VAR_SET_EVT(msg, NULL, NULL);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_NETWORK_CALL_CLOSE;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 2000);
}

lwgsmr_t lwgsm_shutdown() {
    LWGSM_MSG_VAR_DEFINE(msg);
    LWGSM_MSG_VAR_ALLOC(msg, 1);
    LWGSM_MSG_VAR_SET_EVT(msg, NULL, NULL);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_SHUTDOWN;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 2000);
}
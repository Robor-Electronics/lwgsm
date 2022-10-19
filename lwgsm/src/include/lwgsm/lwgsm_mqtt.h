#pragma once
#ifndef LOW_COST_TRACKER_LWGSM_MQTT_H
#define LOW_COST_TRACKER_LWGSM_MQTT_H

#include "lwgsm/lwgsm_types.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

lwgsmr_t lwgsm_mqtt_pub(
        const char *address,
        const char *user,
        const char *topic,
        const char *client_id,
        const char *data,
        size_t length
);

lwgsmr_t lwgsm_http_post(
        const char *address,
        const char *data,
        size_t length
);

//    Request International Mobile Subscriber Identity
        lwgsmr_t lwgsm_request_mobile_subscriber_id(
                char *subscribe_id,
                size_t length,
                lwgsm_api_cmd_evt_fn evt_fn,
                void *evt_arg,
                uint32_t blocking
        );

lwgsmr_t lwgsm_cancel_call();

lwgsmr_t lwgsm_shutdown();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //LOW_COST_TRACKER_LWGSM_MQTT_H

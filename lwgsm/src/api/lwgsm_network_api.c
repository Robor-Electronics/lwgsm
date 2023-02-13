/**
 * \file            lwgsm_network_api.c
 * \brief           API functions for multi-thread network functions
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwGSM - Lightweight GSM-AT library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v0.1.1
 */
#include "lwgsm/lwgsm_network_api.h"
#include "lwgsm/lwgsm_network.h"
#include "lwgsm/lwgsm_private.h"

#if LWGSM_CFG_NETWORK || LWGSM_CFG_NETWORK_CENTERION || __DOXYGEN__

/* Network credentials used during connect operation */
static const char* network_apn;
static const char* network_user;
static const char* network_pass;
static uint32_t network_counter;

/**
 * \brief           Set system network credentials before asking for attach
 * \param[in]       apn: APN domain. Set to `NULL` if not used
 * \param[in]       user: APN username. Set to `NULL` if not used
 * \param[in]       pass: APN password. Set to `NULL` if not used
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t otherwise
 */
lwgsmr_t
lwgsm_network_set_credentials(const char* apn, const char* user, const char* pass) {
    network_apn = apn;
    network_user = user;
    network_pass = pass;

    return lwgsmOK;
}

/**
 * \brief           Request manager to attach to network
 * \note            This function is blocking and cannot be called from event functions
 * \return          \ref lwgsmOK on success (when attached), member of \ref lwgsmr_t otherwise
 */
lwgsmr_t
lwgsm_network_request_attach(void) {
    lwgsmr_t res = lwgsmOK;
    uint8_t do_conn = 0;

    /* Check if we need to connect */
    lwgsm_core_lock();
    if (network_counter == 0) {
        if (!lwgsm_network_is_attached()) {
            do_conn = 1;
        }
    }
    if (!do_conn) {
        ++network_counter;
    }
    lwgsm_core_unlock();

    /* Connect to network */
    if (do_conn) {
        res = lwgsm_network_attach(network_apn, network_user, network_pass, NULL, NULL, 1);
        if (res == lwgsmOK) {
            lwgsm_core_lock();
            ++network_counter;
            lwgsm_core_unlock();
        }
    }
    return res;
}

/**
 * A version of the attach function that does not internally track if the device is still attached or not.
 * @return lwgsmOK on success
 */
lwgsmr_t
lwgsm_network_untracked_attach(void) {
#ifdef LWGSM_CFG_NETWORK_CENTERION
    // Ensure auto attach is disabled before attaching
    const lwgsmr_t status = lwgsm_network_disable_auto_attach(NULL, NULL, 1);
    if (status != lwgsmOK) {
        return status;
    }
#endif

    return lwgsm_network_attach(network_apn, network_user, network_pass, NULL, NULL, 1);
}

/**
 * \brief           Request manager to detach from network
 *
 * If other threads use network, manager will not disconnect from network
 * otherwise it will disable network access
 *
 * \note            This function is blocking and cannot be called from event functions
 * \return          \ref lwgsmOK on success (when attached), member of \ref lwgsmr_t otherwise
 */
lwgsmr_t
lwgsm_network_request_detach(void) {
    lwgsmr_t res = lwgsmOK;
    uint8_t do_disconn = 0;

    /* Check if we need to disconnect */
    lwgsm_core_lock();
    if (network_counter > 0) {
        if (network_counter == 1) {
            do_disconn = 1;
        } else {
            --network_counter;
        }
    }
    lwgsm_core_unlock();

    /* Connect to network */
    if (do_disconn) {
        res = lwgsm_network_detach(NULL, NULL, 1);
        if (res == lwgsmOK) {
            lwgsm_core_lock();
            --network_counter;
            lwgsm_core_unlock();
        }
    }
    return res;
}

lwgsmr_t
lwgsm_network_reset_operator() {
    // Force de-register
    lwgsmr_t res;
    res = lwgsm_operator_set(
            LWGSM_OPERATOR_MODE_DEREGISTER,
            LWGSM_OPERATOR_FORMAT_NUMBER,
            NULL,
            0,
            NULL,
            NULL,
            true
    );
    if(res != lwgsmOK) {
        return res;
    }

    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, true);
    LWGSM_MSG_VAR_SET_EVT(msg, NULL, NULL);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_CGDCONT;
    LWGSM_MSG_VAR_REF(msg).msg.network_attach.apn = network_apn;

    res = lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 2000);
    if(res != lwgsmOK) {
        return res;
    }

    res = lwgsm_operator_set(
            LWGSM_OPERATOR_MODE_AUTO,
            LWGSM_OPERATOR_FORMAT_NUMBER,
            NULL,
            0,
            NULL,
            NULL,
            true
    );
    if(res != lwgsmOK) {
        return res;
    }
}
#endif /* LWGSM_CFG_NETWORK || __DOXYGEN__ */

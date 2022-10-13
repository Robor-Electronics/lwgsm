/**
 * \file    lwgsm_sys_port.h
 * \brief   System dependent functions for Zephyr based operating system
 */
#pragma once

#ifndef LWLIBPROJECT_LWGSM_SYS_PORT_H
#define LWLIBPROJECT_LWGSM_SYS_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#if LWGSM_CFG_OS && !__DOXYGEN__

#include <zephyr/kernel.h>

/**
 * \brief           System mutex type
 *
 * It is used by middleware as base type of mutex.
 */
typedef struct k_mutex lwgsm_sys_mutex_t;

/**
 * \brief           System semaphore type
 *
 * It is used by middleware as base type of mutex.
 */
typedef struct k_sem lwgsm_sys_sem_t;

/**
 * \brief           System message queue type
 *
 * It is used by middleware as base type of mutex.
 */
typedef struct k_mbox lwgsm_sys_mbox_t;

/**
 * \brief           System thread ID type
 */
typedef struct k_thread lwgsm_sys_thread_t;

/**
 * \brief           System thread priority type
 *
 * It is used as priority type for system function,
 * to start new threads by middleware.
 */
typedef int lwgsm_sys_thread_prio_t;

/**
 * \brief           OS timeout value
 *
 * Value returned by operating system functions (mutex wait, sem wait, mbox wait)
 * when it returns timeout and does not give valid value to application
 */
#define LWGSM_SYS_TIMEOUT ((uint32_t)-1)

/**
 * \brief           Default thread priority value used by middleware to start built-in threads
 *
 * Threads can well operate with normal (default) priority and do not require
 * any special feature in terms of priority for prioer operation.
 */
#define LWGSM_SYS_THREAD_PRIO 0

/**
 * \brief           Stack size in units of bytes for system threads
 *
 * It is used as default stack size for all built-in threads.
 */
#define LWGSM_SYS_THREAD_SS   (1024)

#endif /* LWGSM_CFG_OS && !__DOXYGEN__ */

#ifdef __cplusplus
};
#endif

#endif //LWLIBPROJECT_LWGSM_SYS_PORT_H

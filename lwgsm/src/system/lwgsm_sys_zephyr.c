/**
 * \file            lwgsm_sys_freertos.c
 * \brief           System dependant functions for Zephyr RTOS
 */

#include <stdint.h>
#include <string.h>

#include <system/lwgsm_sys.h>

const char *LWGSM_PRODUCE_THREAD_NAME = "lwgsm_produce";
K_THREAD_STACK_DEFINE(lwgsm_produce_stack, LWGSM_SYS_THREAD_SS);

const char *LWGSM_CONSUME_THREAD_NAME = "lwgsm_consume";
K_THREAD_STACK_DEFINE(lwgsm_consume_stack, LWGSM_SYS_THREAD_SS);

// Mutex ID for main protection
struct k_mutex sys_mutex;

uint8_t lwgsm_sys_init(void) {
    return lwgsm_sys_mutex_create(&sys_mutex);
}

uint32_t lwgsm_sys_now(void) {
    return k_uptime_get();
}

uint8_t lwgsm_sys_protect(void) {
    return lwgsm_sys_mutex_lock(&sys_mutex);
}

uint8_t lwgsm_sys_unprotect(void) {
    return lwgsm_sys_mutex_unlock(&sys_mutex);
}

uint8_t lwgsm_sys_mutex_create(lwgsm_sys_mutex_t *p) {
    return k_mutex_init(p) == 0;
}

uint8_t lwgsm_sys_mutex_delete(lwgsm_sys_mutex_t *p) {
    return 1;
}

uint8_t lwgsm_sys_mutex_lock(lwgsm_sys_mutex_t *p) {
    return k_mutex_lock(p, K_FOREVER) == 0;
}

uint8_t lwgsm_sys_mutex_unlock(lwgsm_sys_mutex_t *p) {
    return k_mutex_unlock(p) == 0;
}

uint8_t lwgsm_sys_mutex_isvalid(lwgsm_sys_mutex_t *p) {
    return p != NULL;
}

uint8_t lwgsm_sys_mutex_invalid(lwgsm_sys_mutex_t *p) {
    (void) p;
    return 1;
}

uint8_t lwgsm_sys_sem_create(lwgsm_sys_sem_t *sem, uint8_t count) {
    // When count == 0, it is expected that the semaphore
    // is immediately locked on creation. So we create a
    // semaphore with initial count of 0 and a limit of 1.
    if (count == 0) {
        return k_sem_init(sem, 0, 1) == 0;
    }
    return k_sem_init(sem, count, count) == 0;
}

uint8_t lwgsm_sys_sem_delete(lwgsm_sys_sem_t *sem) {
    k_sem_reset(sem);
    return 1;
}

uint32_t lwgsm_sys_sem_wait(lwgsm_sys_sem_t *sem, uint32_t timeout) {
    int result;
    int64_t t = k_uptime_get();
    result = k_sem_take(sem, timeout == 0 ? K_FOREVER : K_MSEC(timeout));
    return result == 0 ? k_uptime_delta(&t) : -1;
}

uint8_t lwgsm_sys_sem_release(lwgsm_sys_sem_t *sem) {
    k_sem_give(sem);
    return 1;
}

uint8_t lwgsm_sys_sem_isvalid(lwgsm_sys_sem_t *sem) {
    return sem != NULL;
}

uint8_t lwgsm_sys_sem_invalid(lwgsm_sys_sem_t *sem) {
    (void) 0;
    return 1;
}

uint8_t lwgsm_sys_mbox_create(lwgsm_sys_mbox_t *mbox, size_t size) {
    (void) size;
    k_mbox_init(mbox);
    return 1;
}

uint8_t lwgsm_sys_mbox_delete(lwgsm_sys_mbox_t *mbox) {
    return 1;
}

uint32_t lwgsm_sys_mbox_put(lwgsm_sys_mbox_t *mbox, void *message) {
    int64_t t;

    struct k_mbox_msg msg;
    msg.tx_data = message;
    msg.size = 0;
    msg.tx_block.data = NULL;
    msg.tx_target_thread = K_ANY;

    t = k_uptime_get();
    k_mbox_put(mbox, &msg, K_FOREVER);
    return k_uptime_delta(&t);
}

uint32_t lwgsm_sys_mbox_get(lwgsm_sys_mbox_t *mbox, void **message, uint32_t timeout) {
    struct k_mbox_msg rx_msg;
    rx_msg.size = 0;
    rx_msg.rx_source_thread = K_ANY;
    int64_t t = k_uptime_get();
    if (k_mbox_get(mbox, &rx_msg, NULL, timeout == 0 ? K_FOREVER : K_MSEC(timeout))) {
        return LWGSM_SYS_TIMEOUT;
    }
    *message = rx_msg.tx_data;
    return k_uptime_delta(&t);
}

uint8_t lwgsm_sys_mbox_putnow(lwgsm_sys_mbox_t *mbox, void *message) {
    struct k_mbox_msg msg;
    msg.tx_data = message;
    msg.size = 0;
    msg.tx_block.data = NULL;
    msg.tx_target_thread = K_ANY;
    return k_mbox_put(mbox, &msg, K_NO_WAIT) == 0;
}

uint8_t lwgsm_sys_mbox_getnow(lwgsm_sys_mbox_t *mbox, void **message) {
    struct k_mbox_msg rx_msg;
    rx_msg.size = 0;
    rx_msg.rx_source_thread = K_ANY;
    if (k_mbox_get(mbox, &rx_msg, NULL, K_NO_WAIT)) {
        return 0;
    }
    *message = rx_msg.tx_data;
    return 1;
}

uint8_t lwgsm_sys_mbox_isvalid(lwgsm_sys_mbox_t *mbox) {
    return mbox != NULL;
}

uint8_t lwgsm_sys_mbox_invalid(lwgsm_sys_mbox_t *mbox) {
    (void) mbox;
    return 1;
}

// Wrapper to convert between k_thread_entry and lwgsm_sys_thread_fn
void lwgsm_sys_thread_start(void *arg1, void *arg2, void *arg3) {
    (void) arg3;
    ((lwgsm_sys_thread_fn) arg1)(arg2);
}

uint8_t lwgsm_sys_thread_create(
        lwgsm_sys_thread_t *t,
        const char *name,
        lwgsm_sys_thread_fn thread_func,
        void *const arg,
        size_t stack_size,
        lwgsm_sys_thread_prio_t prio
) {
    (void) stack_size;

    if (strncmp(name, LWGSM_PRODUCE_THREAD_NAME, strlen(LWGSM_PRODUCE_THREAD_NAME)) != 0) {
        k_thread_create(
                /*new_thread=*/ t,
                /*stack=*/ lwgsm_produce_stack,
                /*stack_size=*/ K_THREAD_STACK_SIZEOF(lwgsm_produce_stack),
                /*entry=*/ lwgsm_sys_thread_start,
                /*p1=*/ thread_func,
                /*p2=*/ arg,
                /*p3=*/ NULL,
                /*prio=*/ prio,
                /*options=*/ 0,
                /*delay=*/ K_NO_WAIT
        );
        return 1;
    }

    if (strncmp(name, LWGSM_CONSUME_THREAD_NAME, strlen(LWGSM_CONSUME_THREAD_NAME)) != 0) {
        k_thread_create(
                /*new_thread=*/ t,
                /*stack=*/ lwgsm_consume_stack,
                /*stack_size=*/ K_THREAD_STACK_SIZEOF(lwgsm_consume_stack),
                /*entry=*/ lwgsm_sys_thread_start,
                /*p1=*/ thread_func,
                /*p2=*/ arg,
                /*p3=*/ NULL,
                /*prio=*/ prio,
                /*options=*/ 0,
                /*delay=*/ K_NO_WAIT
        );
        return 1;
    }

    return 0;
}

uint8_t lwgsm_sys_thread_terminate(lwgsm_sys_thread_t* t) {
    k_thread_abort(t);
    return 1;
}

uint8_t lwgsm_sys_thread_yield(void) {
    k_yield();
    return true;
}

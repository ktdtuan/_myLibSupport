#include "retryFunc.h"

void retry_function_init(retry_func_t *str_t, uint8_t (*func)(), uint16_t retry, uint32_t interval, uint8_t wait_result, void (*cbErr)(uint8_t result))
{
    timer_set(&str_t->_poll, interval / 10);
    str_t->_function = func;
    str_t->_cbError = cbErr;
    str_t->_wait_result = wait_result;
    str_t->_retry = retry;
}

void retry_function_loop(retry_func_t *str_t)
{
    if (str_t->_retry == 0)
        return;

    if (timer_expired(&str_t->_poll) == 0)
        return;

    if (str_t->_function == 0)
    {
        str_t->_retry = 0;
        timer_stop(&str_t->_poll);
        return;
    }

    uint8_t result = str_t->_function();

    if (result == str_t->_wait_result)
    {
        str_t->_retry = 0;
        return;
    }
    if (--str_t->_retry == 0)
    {
        if (str_t->_cbError != 0)
            str_t->_cbError(result);
        timer_stop(&str_t->_poll);
        return;
    }
    timer_restart(&str_t->_poll);
}
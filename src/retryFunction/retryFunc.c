#include "retryFunc.h"

void ReF_init(retryFunc_t *reFunc, void *handle, uint8_t (*reciver)(void *arg), void *cbResult)
{
    reFunc->_handle = handle;
    reFunc->_reciver = reciver;
    reFunc->_cbResult = cbResult;
}

void ReF_start(retryFunc_t *reFunc, uint32_t interval, uint16_t retry, reFTypeRev_t type, const char *pcs, const char *rev)
{
    if (reFunc->_handle == 0)
        return;
    reFunc->_handle((void *)pcs);
    reFunc->_retry = (retry == 0) ? 1 : retry;
    timer_set(&reFunc->_poll, interval);
}

void ReF_stop(retryFunc_t *reFunc)
{
    reFunc->_retry = 0;
    timer_stop(&reFunc->_poll);
}

void ReF_loop(retryFunc_t *reFunc)
{
    if (reFunc->_retry == 0)
        return;

    // call function check result
    if (reFunc->_reciver(0) != 0)
    {
    }

    // reset time of interval retry
    if (timer_expired(&reFunc->_poll) == 0)
        return;
    timer_restart(&reFunc->_poll);

    // check retry. if there is no retry it's over
    if (--reFunc->_retry == 0)
    {
        if (reFunc->_cbResult != 0)
            reFunc->_cbResult(0);
        timer_stop(&reFunc->_poll);
        return;
    }

    // call function process
    if (reFunc->_handle == 0)
    {
        reFunc->_retry = 0;
        timer_stop(&reFunc->_poll);
        return;
    }
    reFunc->_handle(0);
}

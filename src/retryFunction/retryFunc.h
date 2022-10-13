#ifndef _RETRY_FUNCTION_H__
#define _RETRY_FUNCTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../myLib.h"

typedef struct
{
	uint8_t (*_function)(void);
	uint16_t _retry;
	uint8_t _wait_result;
	timer_virtual_t _poll;
	void (*_cbError)(uint8_t result);
} retry_func_t;

void retry_function_init(retry_func_t *str_t, uint8_t (*func)(), uint16_t retry, uint32_t interval, uint8_t wait_result, void (*cbErr)(uint8_t result));
void retry_function_loop(retry_func_t *str_t);

#ifdef __cplusplus
}
#endif

#endif /* _RETRY_FUNCTION_H__ */
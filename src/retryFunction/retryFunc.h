#ifndef _RETRY_FUNCTION_H__
#define _RETRY_FUNCTION_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "../timeVirtural/timer_virtural.h"

	typedef enum
	{
		reF_Type_Array,
		reF_Type_Byte,
	} reFTypeRev_t;

	typedef struct
	{
		void (*_handle)(void *arg);
		uint8_t (*_reciver)(void *arg);
		void (*_cbResult)(void *arg);
		timer_virtual_t _poll;
		uint16_t _retry;
		reFTypeRev_t _type;
	} retryFunc_t;

	void ReF_init(retryFunc_t *reFunc, void *handle, uint8_t (*reciver)(void *arg), void *cbResult);
	void ReF_start(retryFunc_t *reFunc, uint32_t interval, uint16_t retry, reFTypeRev_t type, const char *pcs, const char *rev);
	void ReF_loop(retryFunc_t *reFunc);

#ifdef __cplusplus
}
#endif

#endif /* _RETRY_FUNCTION_H__ */

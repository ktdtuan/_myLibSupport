#ifndef __GET_STRING_
#define __GET_STRING_

#ifdef __cplusplus
extern "C"
{
#endif

#include "../timeVirtural/timer_virtural.h"
#include "stdint.h"
#include "string.h"

#define MAX_STRING					4//minximum: 2
#define MAX_LEN_STRING				52
#define TIMEOUT_STRING				20//20ms
typedef struct{
	char							string[MAX_STRING][MAX_LEN_STRING];// 2 buffer để nhận dữ liệu
	uint16_t						id_read;//id đọc
	uint16_t						id_write;//id ghi
	uint16_t						count[MAX_STRING];
	timer_virtual_t					timeout;
	//notify
	void							(*void_notify)(char *string, uint16_t size);
}get_string_str;

void get_string_get_input(get_string_str *sGS, char byte); //đặt trong Rx Irq uart
void get_string_init_notify(get_string_str *sGS, void (*void_get_string_notify)(char *string, uint16_t size));// khởi tạo trước main
void get_string_loop_manage(get_string_str *sGS); //đặt trong main.c

#ifdef __cplusplus
}
#endif

#endif	//__GET_STRING_

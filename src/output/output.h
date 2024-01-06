#ifndef __OUTPUT_INTERFACE_H__
#define __OUTPUT_INTERFACE_H__

// #define _EXAMPLE_TEST_OUTPUT
#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "../timeVirtural/timer_virtural.h"

	typedef enum
	{
		OUT_MODE_NO_CONTROL, // no control
		OUT_MODE_OFF,
		OUT_MODE_ON,
		OUT_MODE_BLINK,
	} OUT_MODE_t;

	typedef enum
	{
		OUT_STATE_WAIT_START,
		OUT_STATE_ON,
		OUT_state_finished,
		OUT_STATE_END,
	} OUT_STATE_t;

	typedef enum
	{
		OUT_EVENT_ON_AFTER,
		OUT_EVENT_OFF_AFTER,
		OUT_EVENT_END_BLINK
	} OUT_EVENT_t;

	/**
	 * @brief type function Output hardware control ON/OFF
	 * @param ON_OFF :
	 * 	=1 : set ON
	 * 	=0 : set OFF
	 * 	Note : ON/OFF can be Low/High or High/Low from user choice
	 */
	typedef void (*__OutBaseCtrl)(uint8_t ON_OFF);

	/**
	 * @brief type function callback
	 * @param event It is of type OUT_STATE_ON, OUT_state_finished, OUT_STATE_WAIT or OUT_STATE_END_BLINK when switching from another state to this state immediately
	 * @param cycleNumber : to choose the cycle number (repeat_cnt) wants check event.
	 * 	  cycleNumber == 0 : always checks event with all blink cycle
	 *
	 */
	typedef void (*__OutEventBlinkCallBack)(OUT_EVENT_t event, clock_time_t later_ms);

	typedef struct
	{
		OUT_MODE_t  mode;
		OUT_STATE_t state;
		uint8_t     ena_nonStop;

		uint16_t repeat_num,
				 repeat_cnt;

		clock_time_t val_Duty,
					 val_perriod,
					 val_timeLine;

		timer_virtual_t _timeOut;

		__OutBaseCtrl 			pinBaseCtrl;
		__OutEventBlinkCallBack cb_eventFunction;

	} OUTPUT_t;

	void OUTPUT_setOff(OUTPUT_t *tmp, clock_time_t delayBegin_ms);
	void OUTPUT_setOn(OUTPUT_t *tmp, clock_time_t delayBegin_ms);
	void OUTPUT_setBlink(OUTPUT_t *tmp, uint8_t repeat, clock_time_t dutyON_ms, clock_time_t perriod_ms, uint8_t enable_nonStop);

	// Handle with 1ms interrupt
	void OUTPUT_blinkHandle(OUTPUT_t *tmp);

	uint8_t INPUT_readHanlde(uint8_t inputStt, uint16_t lowSample_ms, uint16_t highSample_ms);
	/*				<---------------------------------------perriod_ms----------------------------------->
	 *				<----------------dutyON_ms----------------> <---------(perriod_ms - dutyON_ms)-------> => one cycle
	 * State		<--------------OUT_STATE_ON---------------> <--------------OUT_state_finished-----------><OUT_STATE_REPEAT> ...
	 *
	 * Pulse logic 	|----------------.-------------------------|_____________________.____________________| => repeat_num again  =>  _________|(end)
	 * 				^	         ^	       	           ^		         ^						  ^
	 * Event: 	 	OUT_EVENT_ON	    OUT_EVENT_ON_DELAY        OUT_EVENT_OFF       OUT_EVENT_OFF_DELAY				   OUT_EVENT_END_BLINK
	 */

#ifdef __cplusplus
}
#endif

#endif

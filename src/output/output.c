#include "output.h"

void OUTPUT_setOff(OUTPUT_t *tmp, clock_time_t delayBegin_ms)
{
	if (tmp->pinBaseCtrl)
	{
		tmp->mode = OUT_MODE_OFF;
		if (delayBegin_ms)
		{
			tmp->state = OUT_STATE_WAIT_START;
			timer_set(&tmp->_timeOut, delayBegin_ms);
		}
		else
		{
			tmp->state = OUT_state_finished;
			tmp->pinBaseCtrl(0);
			timer_stop(&tmp->_timeOut);
			tmp->val_timeLine = 0;
			// callback event OFF
			if (tmp->cb_eventFunction)
			{
				tmp->cb_eventFunction(OUT_EVENT_OFF_AFTER, 0);
			}
		}
	}
}

/**
 * This function controls OFF
 */
void OUTPUT_setOn(OUTPUT_t *tmp, clock_time_t delayBegin_ms)
{
	if (tmp->pinBaseCtrl)
	{
		tmp->mode = OUT_MODE_ON;
		if (delayBegin_ms)
		{
			tmp->state = OUT_STATE_WAIT_START;
			timer_set(&tmp->_timeOut, delayBegin_ms);
		}
		else
		{
			tmp->state = OUT_state_finished;
			tmp->pinBaseCtrl(1);
			timer_stop(&tmp->_timeOut);
			tmp->val_timeLine = 0;
			// callback event OFF
			if (tmp->cb_eventFunction)
			{
				tmp->cb_eventFunction(OUT_EVENT_ON_AFTER, 0);
			}
		}
	}
}

void OUTPUT_setBlink(OUTPUT_t *tmp, uint8_t repeat, clock_time_t dutyON_ms, clock_time_t perriod_ms, uint8_t nonStop)
{

	if (tmp->pinBaseCtrl)
	{
		// Wrong parameter
		if (repeat == 0)
			return;
		if (dutyON_ms == 0)
			return;
		if (perriod_ms <= dutyON_ms)
			return;

		tmp->pinBaseCtrl(1);
		tmp->mode = OUT_MODE_BLINK;
		tmp->state = OUT_STATE_ON;
		tmp->ena_nonStop = nonStop;
		tmp->repeat_num = repeat;
		tmp->repeat_cnt = 0;
		tmp->val_Duty = dutyON_ms;
		tmp->val_perriod = perriod_ms;
		timer_set(&tmp->_timeOut, tmp->val_Duty);
		tmp->val_timeLine = 0;
		// callback event ON when start blink
		if (tmp->cb_eventFunction)
		{
			tmp->cb_eventFunction(OUT_EVENT_ON_AFTER, 0);
		}
	}
}

void OUTPUT_blinkHandle(OUTPUT_t *tmp)
{

	if (tmp->pinBaseCtrl)
	{
		if (tmp->mode != OUT_MODE_NO_CONTROL)
			tmp->val_timeLine++;

		switch (tmp->mode)
		{
		case OUT_MODE_OFF:
			if (timer_expired(&tmp->_timeOut))
			{
				timer_stop(&tmp->_timeOut);
				tmp->state = OUT_state_finished;
				tmp->pinBaseCtrl(0);
				tmp->val_timeLine = 0;
			}
			// call back function
			if (tmp->cb_eventFunction)
			{
				if (tmp->state != OUT_STATE_WAIT_START)
					tmp->cb_eventFunction(OUT_EVENT_OFF_AFTER, tmp->val_timeLine);
			}
			break;

		case OUT_MODE_ON:
			if (timer_expired(&tmp->_timeOut))
			{
				timer_stop(&tmp->_timeOut);
				tmp->state = OUT_STATE_ON;
				tmp->pinBaseCtrl(1);
				tmp->val_timeLine = 0;
			}

			if (tmp->state == OUT_STATE_WAIT_START)
				return;
			// call back function
			if (tmp->cb_eventFunction)
			{
				if (tmp->state != OUT_STATE_WAIT_START)
					tmp->cb_eventFunction(OUT_EVENT_ON_AFTER, tmp->val_timeLine);
			}
			break;

		case OUT_MODE_BLINK:
			switch (tmp->state)
			{
			case OUT_STATE_ON:
				if (timer_expired(&tmp->_timeOut))
				{
					tmp->state = OUT_state_finished;
					tmp->pinBaseCtrl(0);
					timer_set(&tmp->_timeOut, (tmp->val_perriod - tmp->val_Duty));
					tmp->val_timeLine = 0;
				}

				// call back function
				if (tmp->cb_eventFunction)
				{
					uint8_t event;
					event = (tmp->val_timeLine) ? OUT_EVENT_ON_AFTER : OUT_EVENT_OFF_AFTER;
					tmp->cb_eventFunction(event, tmp->val_timeLine);
				}
				break;

			case OUT_state_finished:
				if (timer_expired(&tmp->_timeOut))
				{
					// neu nonStop thi reset counter
					if (tmp->ena_nonStop)
						tmp->repeat_cnt = 0;
					else
						tmp->repeat_cnt++;

					// dừng lại
					if (tmp->repeat_cnt == tmp->repeat_num)
					{
						tmp->mode = OUT_MODE_NO_CONTROL;
						tmp->state = OUT_state_finished;
						tmp->pinBaseCtrl(0);
						timer_stop(&tmp->_timeOut);
						tmp->val_timeLine = 0;
						// call back function
						if (tmp->cb_eventFunction)
						{
							tmp->cb_eventFunction(OUT_EVENT_END_BLINK, tmp->val_timeLine);
						}
						return;
					}
					// tiếp tục
					else
					{
						tmp->state = OUT_STATE_ON;
						tmp->pinBaseCtrl(1);
						timer_set(&tmp->_timeOut, tmp->val_Duty);
						tmp->val_timeLine = 0;
					}
				}

				// call back function
				if (tmp->cb_eventFunction)
				{
					uint8_t event;
					event = (tmp->val_timeLine) ? OUT_EVENT_OFF_AFTER : OUT_EVENT_ON_AFTER;
					tmp->cb_eventFunction(event, tmp->val_timeLine);
				}

				break;

			default:
				break;
			} // end switch check state of mode blink
			break;

		default:
			break;
		} // end switch check mode
	}	  // check Pointer
}

uint8_t INPUT_readHanlde(uint8_t inputStt, uint16_t lowSample_ms, uint16_t highSample_ms)
{
	static uint16_t lowCnt = 0, highCnt = 0;
	static uint8_t reti = 0xFF;

	// Khởi tạo giá trị ban đầu giống với inputStt
	if (reti == 0xFF)
		reti = (inputStt) ? 1 : 0;

	if (inputStt)
	{
		lowCnt = 0;
		if (++highCnt >= highSample_ms)
		{
			highCnt = 0;
			reti = inputStt;
		}
	}
	else
	{
		highCnt = 0;
		if (++lowCnt >= lowSample_ms)
		{
			lowCnt = 0;
			reti = 0;
		}
	}

	return reti;
}

#ifdef EXAMPLE_TEST_OUTPUT
OUTPUT_t UserPin1;

/******** DECLARE ***********/
void PIN1_BaseCtrl(uint8_t ON_OFF);
void PIN2_BaseCtrl(uint8_t ON_OFF);
void Pin2_checkEventPin1(OUT_EVENT_t event, clock_time_t later_ms);
void userInitOutput();
void userOutputHanlde();

/******** FUNCTIONS ***********/
void PIN1_BaseCtrl(uint8_t ON_OFF) { (ON_OFF) ? HAL_GPIO_WritePin(ULN_SOL_GPIO_Port, ULN_SOL_Pin, 1) : HAL_GPIO_WritePin(ULN_SOL_GPIO_Port, ULN_SOL_Pin, 0); }

void PIN2_BaseCtrl(uint8_t ON_OFF) { (ON_OFF) ? HAL_GPIO_WritePin(LED_STT_GPIO_Port, LED_STT_Pin, 1) : HAL_GPIO_WritePin(LED_STT_GPIO_Port, LED_STT_Pin, 0); }

void userInitOutput()
{
	UserPin1.pinBaseCtrl = &PIN1_BaseCtrl;
	UserPin1.cb_eventFunction = &Pin2_checkEventPin1;

	//  OUTPUT_setOn(&UserPin1, 5000);
	// OUTPUT_setOn(&UserPin1, 0);

	OUTPUT_setBlink(&UserPin1, 10, 50, 100, 0); // non-stop
	//  OUTPUT_setBlink(&UserPin1, 1, 60, 100, 1); //repeated 100
}

// put in Irq timer
void userOutputHanlde()
{
	OUTPUT_blinkHandle(&UserPin1);
}

// Callback control Pin2
void Pin2_checkEventPin1(OUT_EVENT_t event, clock_time_t later_ms)
{
	switch (event)
	{
	case OUT_EVENT_ON_AFTER:
		if (later_ms == 25)
			PIN2_BaseCtrl(1);
		break;

	case OUT_EVENT_OFF_AFTER:
		if (later_ms == 25)
			PIN2_BaseCtrl(0);
		break;

	default:
		break;
	}
}

/**
 * @brief  SYSTICK callback.
 * @retval None
 */
void HAL_SYSTICK_Callback(void)
{
	// 1ms
	timer_periodic_poll();
	userOutputHanlde();
}

#endif

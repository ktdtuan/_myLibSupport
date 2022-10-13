#ifndef __FIFO_H__
#define __FIFO_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stdio.h"

typedef struct
{
    uint8_t *buff;             // buffer lưu dữ liệu tạm
    uint16_t buff_size;        // độ dài của buffer lưu lựu
    uint16_t id_rd, id_wr;     // vị trí ghi và lấy dữ liệu hiện tại
    void (*cb_overload)(void); // function callback overload buff
} fifo_t;

uint8_t fifo_init(fifo_t *fifo, uint16_t len_buff, void (*cb_over)(void));
void fifo_deinit(fifo_t *fifo);

void fifo_give_byte(fifo_t *fifo, uint8_t data);
uint8_t fifo_give_array(fifo_t *fifo, uint8_t *data, uint16_t length);

uint16_t fifo_availeble(fifo_t *fifo);
uint8_t fifo_read_byte(fifo_t *fifo);
void fifo_read_array(fifo_t *fifo, uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* __FIFO_H__ */
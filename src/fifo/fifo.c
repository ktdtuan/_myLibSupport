#include "fifo.h"

uint8_t fifo_init(fifo_t *fifo, uint16_t len_buff, void (*cb_over)(void))
{
    fifo->buff = (uint8_t *)malloc(len_buff);
    if (fifo->buff == NULL)
        return 0;
    fifo->buff_size = len_buff;
    fifo->cb_overload = cb_over;
    return 1;
}
void fifo_deinit(fifo_t *fifo)
{
    free(fifo->buff);
}

/**
 * @brief
 *
 * @param fifo struct fifo processing
 * @return uint16_t length free now
 */
uint16_t size_buff_free(fifo_t *fifo)
{
    int16_t differen_size = fifo->id_rd - fifo->id_wr - 1; // 1 offset zero origin
    return (differen_size > 0) ? (differen_size) : (fifo->buff_size + differen_size);
}

/**
 * @brief
 *
 * @param fifo struct fifo processing
 * @param length size of data prepare write buffer
 * @return uint16_t value size overload
 */
uint16_t is_overload(fifo_t *fifo, uint16_t length)
{
    int16_t len_over = length - size_buff_free(fifo);
    return (len_over >= 0) ? len_over + 1 : 0; // offset 1 when len_over = 0
}

void fifo_give_byte(fifo_t *fifo, uint8_t data)
{
    if (fifo->buff == NULL)
        return;

    // check notify overload buffer temprature
    if (is_overload(fifo, 1) != 0 && fifo->cb_overload != NULL)
        fifo->cb_overload();

    // give one byte to buffer temprature
    fifo->buff[fifo->id_wr] = data;

    // offset index write one counter
    if (++fifo->id_wr >= fifo->buff_size)
        fifo->id_wr = 0;
}

/**
 * @brief
 *
 * @param fifo
 * @param data array data write buffer temprature
 * @param length size of data prepare write buffer temprature
 * @return uint16_t is length overload of size buffer temprature
 */
uint16_t give_array(fifo_t *fifo, uint8_t *data, uint16_t length)
{
    // error when size data
    uint16_t length_overload = 0;
    if (length > fifo->buff_size)
    {
        length_overload = length - fifo->buff_size;
        length = fifo->buff_size;
    }

    if (fifo->id_wr + length <= fifo->buff_size)
        memcpy(&fifo->buff[fifo->id_wr], data, length);
    else
    {
        uint16_t size_append = fifo->buff_size - fifo->id_wr;
        memcpy(&fifo->buff[fifo->id_wr], data, size_append);
        memcpy(fifo->buff, &data[size_append], length - size_append);
    }

    // caculator write index
    fifo->id_wr += length;
    if (fifo->id_wr >= fifo->buff_size)
        fifo->id_wr -= fifo->buff_size;

    return length_overload;
    // return 0;
}

uint8_t fifo_give_array(fifo_t *fifo, uint8_t *data, uint16_t length)
{
    if (fifo->buff == NULL)
        return 0;
    if (length == 0)
        return 0;

    uint16_t len_free = size_buff_free(fifo);
    if (len_free == 0 || len_free == fifo->buff_size)
    {
        // notify overload buffer temprature
        if (fifo->cb_overload != NULL)
            fifo->cb_overload();

        // re-check after callback
        if ((len_free = size_buff_free(fifo)) == 0)
            len_free = fifo->buff_size;
    }

    // if length of data < length free in the buffer temprature
    if (length <= len_free)
        give_array(fifo, data, length);
    else
    {
        give_array(fifo, data, len_free);
        data += len_free;
        length -= len_free;

        // the rest data then process continue
        fifo_give_array(fifo, data, length);
    }
    return 1;
}

uint16_t fifo_availeble(fifo_t *fifo)
{
    if (fifo->id_rd == fifo->id_wr)
        return 0;

    int16_t size_append = fifo->id_wr - fifo->id_rd;
    return (size_append > 0) ? size_append : (fifo->buff_size + size_append);
}
uint8_t fifo_read_byte(fifo_t *fifo)
{
    uint8_t data = fifo->buff[fifo->id_rd];
    if (++fifo->id_rd >= fifo->buff_size)
        fifo->id_rd = 0;
    return data;
}
void fifo_read_array(fifo_t *fifo, uint8_t *data, uint16_t length)
{
    if (length + fifo->id_rd >= fifo->buff_size)
    {
        uint16_t size_append = fifo->buff_size - fifo->id_rd;
        memcpy(data, &fifo->buff[fifo->id_rd], size_append);
        memcpy(&data[size_append], fifo->buff, length - size_append);
    }
    else
        memcpy(data, &fifo->buff[fifo->id_rd], length);

    fifo->id_rd += length;
    if (fifo->id_rd >= fifo->buff_size)
        fifo->id_rd -= fifo->buff_size;
}

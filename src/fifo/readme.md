This is a function give buffer. That can be used to store byte array or one byte. It is written in C languare and you can compiled with any landguare. When init dynamic memory and retrun feedback true or false, you can check it.
The library have some functio:
+ Give one byte or muti byte to memory temprature. Use can overload size dynamic memory init, and now the function to callback overload
+ Read one byte or muti byte from memory temprature


Example use:

/**
The example use fifo mode write muti byte and read muti byte
*/
#include "fifo.h"

fifo_t fifo_example;

void cb_overload_ram(void)
{
    Serial.println("overload size of buffer temprature");

    uint16_t length = fifo_availeble(&fifo_example);
    uint8_t *buffer = (uint8_t *)malloc(length + 1);
    memset(buffer, 0, sizeof(buffer));

    fifo_read_array(&fifo_example, buffer, length);

    Serial.printf("==> %d: %s\r\n", length, (char *)buffer);
    free(buffer);
}

void serialReciver(void)
{
    if ((uint8_t size_data = SERIAL_CMD.available()) != 0)
    {
        uint8_t *buffer = (uint8_t *)malloc(size_data + 1);

        Serial.read(buffer, size_data);
        fifo_give_array(&fifo_example, buffer, size_data);

        free(buffer);
    }
}

void readDataInFifo(void)
{
    if ((uint8_t size_data = fifo_availeble(&fifo_example) != 0))
    {
        uint8_t *buffer = (uint8_t *)malloc(size_data + 1);
        memset(buffer, 0, sizeof(size_data));

        fifo_read_array(&fifo_example, buffer, size_data);

        Serial.printf("data read: %s", (char *)buffer);

        free(buffer);
    }
}

void init()
{
    Serial.begin(115200);

    fifo_init(&fifo_example, 10, cb_overload_ram); // length of buffer temprature are 10 byte
}

void loop()
{
    //read data in serial and write fifo library
    serialReciver();

    //read data in fifo library write serial
    readDataInFifo();
}

//The end
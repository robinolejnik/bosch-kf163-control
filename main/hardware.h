#ifndef _HARDWARE_H_
#define _HARDWARE_H_

#include "driver/gpio.h"

void gpio_init(void);

#define GPIO_PTT_SENSE  GPIO_NUM_35
#define GPIO_PTT_OUT    GPIO_NUM_27
#define GPIO_SQL_SENSE  GPIO_NUM_13
#define GPIO_SQL_OUT    GPIO_NUM_12

#define GPIO_LED        GPIO_NUM_22
#define GPIO_MUTE       GPIO_NUM_23

#define GPIO_ST1        GPIO_NUM_14
#define GPIO_ST2        GPIO_NUM_19

#define GPIO_SR_CLK     GPIO_NUM_32
#define GPIO_SR_IN      GPIO_NUM_34
#define GPIO_SR_OUT     GPIO_NUM_2
#define GPIO_SR_LATCH   GPIO_NUM_26

#define GPIO_DIGX_DIV   GPIO_NUM_18
#define GPIO_DIGX_MA0   GPIO_NUM_39
#define GPIO_DIGX_MA1   GPIO_NUM_36
#define GPIO_DIGX_D0    GPIO_NUM_4
#define GPIO_DIGX_D1    GPIO_NUM_16
#define GPIO_DIGX_D2    GPIO_NUM_17
#define GPIO_DIGX_D3    GPIO_NUM_5

#endif // _HARDWARE_H_

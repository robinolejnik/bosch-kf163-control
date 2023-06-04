#include "hardware.h"

void gpio_init(void) {
    gpio_set_direction(GPIO_PTT_SENSE, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_PTT_OUT, GPIO_MODE_OUTPUT);

    gpio_set_direction(GPIO_SQL_SENSE, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_SQL_OUT, GPIO_MODE_OUTPUT);

    gpio_set_direction(GPIO_MUTE, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_LED, GPIO_MODE_OUTPUT);

    gpio_set_direction(GPIO_DIGX_MA0, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_DIGX_MA1, GPIO_MODE_INPUT);
    gpio_set_direction(GPIO_DIGX_D0, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DIGX_D1, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DIGX_D2, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DIGX_D3, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_DIGX_DIV, GPIO_MODE_OUTPUT);
}

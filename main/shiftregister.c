#include "shiftregister.h"
#include "hardware.h"
#include "driver/gptimer.h"
#include "driver/gpio.h"
#include <stdint.h>

static volatile uint8_t serial_direction = 0;
static volatile uint8_t bit_counter = 0;
static volatile uint16_t out = 0;
static volatile uint16_t in = 0;
static volatile uint16_t in_latched = 0;

static bool timer_isr(gptimer_handle_t timer, const gptimer_alarm_event_data_t *edata, void *user_ctx) {
    if(bit_counter%2==0) { // falling clock edge
        gpio_set_level(GPIO_SR_CLK, 0);
        gpio_set_level(GPIO_SR_LATCH, serial_direction);
        gpio_set_level(GPIO_SR_OUT, ((out >> ((bit_counter-2)/2)) & 0x0001));
        in |= ((gpio_get_level(GPIO_SR_IN) & 0x0001) << (bit_counter / 2));
    }
    else { // rising clock edge
        gpio_set_level(GPIO_SR_CLK, 1);
    }

    bit_counter--;

    if(bit_counter==0) { // swap direction and reset counter
        bit_counter = 32;
        if(serial_direction==DIRECTION_READ) {
            // process input
            in_latched = in;
            in = 0;
        }
        serial_direction = !serial_direction;
    }

    return false;
}

void sr_out(uint16_t value) {
    out = value;
}

uint16_t sr_in(void) {
    return in_latched;
}

void sr_init() {
    gpio_set_direction(GPIO_SR_CLK, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_SR_LATCH, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_SR_OUT, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_SR_IN, GPIO_MODE_INPUT);

    gpio_set_level(GPIO_SR_CLK, 0);
    gpio_set_level(GPIO_SR_LATCH, 0);
    gpio_set_level(GPIO_SR_OUT, 0);

    gptimer_handle_t timer_handle = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 100000,
    };
    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer_handle));

    gptimer_alarm_config_t alarm_config = {
        .reload_count = 0,
        .alarm_count = 10,
        .flags.auto_reload_on_alarm = true,
    };
    ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_handle, &alarm_config));

    gptimer_event_callbacks_t callbacks = {
        .on_alarm = timer_isr,
    };
    ESP_ERROR_CHECK(gptimer_register_event_callbacks(timer_handle, &callbacks, NULL));

    ESP_ERROR_CHECK(gptimer_enable(timer_handle));
    ESP_ERROR_CHECK(gptimer_start(timer_handle));
}

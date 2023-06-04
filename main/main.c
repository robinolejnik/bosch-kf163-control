#include "hardware.h"
#include "pll.h"
#include "shiftregister.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <string.h>
#include <stdio.h>

static uint16_t rx_pll = 0;
static uint16_t tx_pll = 0;

void setrxfreq(uint32_t freq) {
	if(freq>100000000) {
		rx_pll = freq2pll(freq - 21400000);
	}
	else {
		rx_pll = freq2pll(freq + 21400000);
	}
}

int settxfreq(uint32_t freq) {
	if(gpio_get_level(GPIO_PTT_SENSE)) {
		return -1;
	}
	tx_pll = freq2pll(freq);
	return 0;
}

uint32_t text2freq(const char *s) {
	uint32_t freq = 0;
	while(*s!=0) {
		freq *= 10;
		freq += *s++ - 48;
	}
	return freq;
}

void app_main(void) {
    gpio_init();
	sr_init();

	setrxfreq(text2freq("167940000"));
	settxfreq(text2freq("167940000"));

	while(1) {
		if(gpio_get_level(GPIO_PTT_SENSE)) {
			sr_out(rx_pll);
		}
		else {
			sr_out(tx_pll);
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

#include "hardware.h"
#include "pll.h"
#include "shiftregister.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <string.h>
#include <stdio.h>

#define STATE_RX 0
#define STATE_TX 1
#define STATE_SQL_CLOSE 0
#define STATE_SQL_OPEN  1

static uint16_t rx_pll = 0;
static uint16_t tx_pll = 0;

static uint8_t trx_state = STATE_RX;
static uint8_t sql_state = STATE_SQL_CLOSE;

void setrxfreq(uint32_t freq) {
	if(freq>100000000) {
		rx_pll = freq2pll(freq - 21400000);
	}
	else {
		rx_pll = freq2pll(freq + 21400000);
	}
}

int settxfreq(uint32_t freq) {
	if(!gpio_get_level(GPIO_PTT_SENSE)) {
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
	sr_out(rx_pll);

	while(1) {
		if(gpio_get_level(GPIO_PTT_SENSE) && trx_state == STATE_TX) {
			trx_state = STATE_RX;
			sr_out(rx_pll);
			printf("RX\n");
		}
		else if(!gpio_get_level(GPIO_PTT_SENSE) && trx_state == STATE_RX) {
			trx_state = STATE_TX;
			sr_out(tx_pll);
			printf("TX\n");
		}

		if(!gpio_get_level(GPIO_SQL_SENSE) && sql_state == STATE_SQL_CLOSE) {
			sql_state = STATE_SQL_OPEN;
			gpio_set_level(GPIO_LED, 1);
			printf("SQL open\n");
		}
		else if(gpio_get_level(GPIO_SQL_SENSE) && sql_state == STATE_SQL_OPEN) {
			sql_state = STATE_SQL_CLOSE;
			gpio_set_level(GPIO_LED, 0);
			printf("SQL close\n");
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

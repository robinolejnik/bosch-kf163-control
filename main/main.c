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

static uint32_t frequencies[] = {
	149025000,
	149037500,
	149050000,
	149087500,
	149100000,
	149112500
};

static uint8_t channel = 0;

void app_main(void) {
	uint8_t button_state = 0;
    gpio_init();
	shiftregister_init();

	rx_pll = freq2pll(frequencies[channel] + 21400000);
	tx_pll = freq2pll(frequencies[channel]);
	shiftregister_out(rx_pll);

	while(1) {
		if(gpio_get_level(GPIO_PTT_SENSE) && trx_state == STATE_TX) {
			trx_state = STATE_RX;
			shiftregister_out(rx_pll);
		}
		else if(!gpio_get_level(GPIO_PTT_SENSE) && trx_state == STATE_RX) {
			trx_state = STATE_TX;
			shiftregister_out(tx_pll);
		}

		if(!gpio_get_level(GPIO_SQL_SENSE) && sql_state == STATE_SQL_CLOSE) {
			sql_state = STATE_SQL_OPEN;
			gpio_set_level(GPIO_LED, 1);
		}
		else if(gpio_get_level(GPIO_SQL_SENSE) && sql_state == STATE_SQL_OPEN) {
			sql_state = STATE_SQL_CLOSE;
			gpio_set_level(GPIO_LED, 0);
		}

		uint16_t shiftregister_in_state = ~shiftregister_in();

		if(shiftregister_in_state & 0x0100 && button_state == 0 && gpio_get_level(GPIO_PTT_SENSE)) {
			button_state = 1;
			channel++;
			if(channel == (sizeof(frequencies)/sizeof(frequencies[0]))) {
				channel = 0;
			}
			shiftregister_out(0); // turn off transceiver
			gpio_set_level(GPIO_LED, 0);

			printf("Channel %u: %lu\n", channel + 1, frequencies[channel]);

			for(uint8_t i=0;i<=channel;i++) {
				vTaskDelay(250 / portTICK_PERIOD_MS);
				gpio_set_level(GPIO_LED, 1);
				vTaskDelay(200 / portTICK_PERIOD_MS);
				gpio_set_level(GPIO_LED, 0);
			}
			vTaskDelay(250 / portTICK_PERIOD_MS);

			rx_pll = freq2pll(frequencies[channel] - 21400000);
			tx_pll = freq2pll(frequencies[channel]);
			shiftregister_out(rx_pll);
		}
		else if(!(shiftregister_in_state & 0x0100)) {
			button_state = 0;
		}
	}
}

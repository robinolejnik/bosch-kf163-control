#include <stdio.h>
#include <string.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include "hardware.h"

static const char* TAG = "KF163";

spi_device_handle_t spi;

unsigned char txstate = 0;
uint32_t rxfreq = 0;
uint32_t txfreq = 0;

void setpllfreq(uint32_t freq) {
	uint16_t data = 0;
	uint8_t shift = 1;
	if(freq % 12500==0 && freq>110000000)
		freq /= 12500;
	else if(freq % 10000==0 && freq>110000000) {
		data |= 0x1000;
		freq /= 10000;
	}
	else if(freq % 5000==0) {
		data |= 0x1000;
		freq /= 5000;
		shift = 2;
	}

	if(freq>=(25600/shift))
		freq-=(25600/shift);
	
	if(freq>=(12800/shift)) {
		data |= 0x2000;
		freq-=(12800/shift);
	}
	
	if(freq>=(6400/shift)) {
		data |= 0x4000;
		freq-=(6400/shift);
	}
	
	if(freq>=(3200/shift)) {
		data |= 0x8000;
		freq-=(3200/shift);
	}
	
	if(freq>=(1600/shift)) {
		data |= 0x0100;
		freq-=(1600/shift);
	}
	
	if(freq>=(800/shift)) {
		data |= 0x0200;
		freq-=(800/shift);
	}
	
	if(freq>=(400/shift)) {
		data |= 0x0400;
		freq-=(400/shift);
	}
	
	if(freq>=(200/shift)) {
		data |= 0x0800;
		freq-=(200/shift);
	}
	
	if(freq>=100 && shift==1) {
		data |= 0x0010;
		freq-=100;
	}
	else if(shift!=1)
		data |= 0x0010;

	if(freq>=64) {
		data |= 0x0020;
		freq-=64;
	}
	
	if(freq>=32) {
		data |= 0x0040;
		freq-=32;
	}
	
	if(freq>=16) {
		data |= 0x0080;
		freq-=16;
	}
	
	if(freq>=8) {
		data |= 0x0004;
		freq-=8;
	}
	
	if(freq>=4) {
		data |= 0x0008;
		freq-=4;
	}
	
	if(freq>=2) {
		data |= 0x0002;
		freq-=2;
	}
	
	if(freq>=1) {
		data |= 0x0001;
		freq-=1;
	}
	// TODO: write to shift register
}

void setrxfreq(uint32_t freq) {
	if(freq>100000000) {
		rxfreq = freq -= 21400000;
	}
	else {
		rxfreq = freq += 21400000;
	}
	
	if(gpio_get_level(GPIO_PTT_SENSE)) {
		setpllfreq(rxfreq);
	}
}

void settxfreq(uint32_t freq) {
	txfreq = freq;
	if(!gpio_get_level(GPIO_PTT_SENSE)) {
		setpllfreq(txfreq);
	}
}

uint32_t text2freq(const char *s) {
	uint32_t freq = 0;
	while(*s!=0) {
		freq *= 10;
		freq += *s++ - 48;
	}
	return freq;
}

static void app_pll(void *pvParameter) {
	while(1) {
		if(gpio_get_level(GPIO_PTT_SENSE)) {
			setpllfreq(rxfreq);
		}
		else {
			setpllfreq(txfreq);
		}

		spi_transaction_t t;
		memset(&t, 0 , sizeof(t));
		t.length = 16;
		t.flags = SPI_TRANS_USE_TXDATA | SPI_TRANS_USE_RXDATA;
		t.tx_data[0] = 0xaa;
		t.tx_data[1] = 0xaa;
		ESP_ERROR_CHECK(spi_device_polling_transmit(spi, &t));
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void spi_pre_transfer_callback(spi_transaction_t *t) {
	gpio_set_level(GPIO_SR_LATCH, 0);
}

void spi_post_transfer_callback(spi_transaction_t *t) {
	gpio_set_level(GPIO_SR_LATCH, 1);
}

void app_main(void) {
    ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(nvs_flash_init());
    gpio_init();

	spi_bus_config_t spi_config = {
		.mosi_io_num = GPIO_SR_OUT,
		.miso_io_num = GPIO_SR_IN,
		.sclk_io_num = GPIO_SR_CLK,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.max_transfer_sz = 2
	};
	ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &spi_config, SPI_DMA_DISABLED));

	spi_device_interface_config_t spi_dev_config = {
		.clock_speed_hz = 10000,
		.mode = 0,
		.spics_io_num = -1,
		.queue_size = 1,
		.pre_cb = spi_pre_transfer_callback,
		.post_cb = spi_post_transfer_callback
	};

	ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &spi_dev_config, &spi));
	
	setrxfreq(text2freq("145500000"));
	settxfreq(text2freq("145500000"));

	xTaskCreatePinnedToCore(app_pll, "app_pll", 40000, NULL, 15, NULL, APP_CPU_NUM);
}

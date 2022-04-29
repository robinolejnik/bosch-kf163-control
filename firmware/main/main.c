#include <stdio.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "esp_event.h"
#include "freertos/task.h"

#include "sdkconfig.h"
#include "driver/gpio.h"

#include "hardware.h"

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
	}
}

static void app_channel(void *pvParameter) {
}

void app_main(void) {
    ESP_ERROR_CHECK(esp_event_loop_create_default());
	ESP_ERROR_CHECK(nvs_flash_init());
    gpio_init();

	setrxfreq(text2freq("145500000"));
	settxfreq(text2freq("145500000"));

	xTaskCreatePinnedToCore(app_pll, "app_pll", 4000, NULL, 15, NULL, APP_CPU_NUM);
	xTaskCreatePinnedToCore(app_channel, "app_channel", 4000, NULL, 15, NULL, APP_CPU_NUM);
}

/*
read ptt sense, set pll
watch channel, set freq




*/
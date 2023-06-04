#include "pll.h"
#include <stdint.h>

uint16_t freq2pll(uint32_t freq) {
	uint16_t data = 0xffff;
	uint8_t shift = 1;
	if(freq % 12500==0 && freq>110000000) {
		freq /= 12500;
	}
	else if(freq % 10000==0 && freq>110000000) {
		data &= ~0x8000;
		freq /= 10000;
	}
	else if(freq % 5000==0) {
		data &= ~0x8000;
		freq /= 5000;
		shift = 2;
	}

	if(freq>=(25600/shift)) {
		freq -= (25600/shift);
	}
	
	if(freq>=(12800/shift)) {
		data &= ~0x4000;
		freq -= (12800/shift);
	}
	
	if(freq>=(6400/shift)) {
		data &= ~0x2000;
		freq -= (6400/shift);
	}
	
	if(freq>=(3200/shift)) {
		data &= ~0x1000;
		freq -= (3200/shift);
	}
	
	if(freq>=(1600/shift)) {
		data &= ~0x0800;
		freq -= (1600/shift);
	}
	
	if(freq>=(800/shift)) {
		data &= ~0x0400;
		freq -= (800/shift);
	}
	
	if(freq>=(400/shift)) {
		data &= ~0x0200;
		freq -= (400/shift);
	}
	
	if(freq>=(200/shift)) {
		data &= ~0x0100;
		freq -= (200/shift);
	}
	
	if(freq>=100 && shift==1) {
		data &= ~0x0080;
		freq -= 100;
	}
	else if(shift!=1) {
		data &= ~0x0080;
	}

	if(freq>=64) {
		data &= ~0x0040;
		freq -= 64;
	}
	
	if(freq>=32) {
		data &= ~0x0020;
		freq -= 32;
	}
	
	if(freq>=16) {
		data &= ~0x0010;
		freq -= 16;
	}
	
	if(freq>=8) {
		data &= ~0x0008;
		freq -= 8;
	}
	
	if(freq>=4) {
		data &= ~0x0004;
		freq -= 4;
	}
	
	if(freq>=2) {
		data &= ~0x0002;
		freq -= 2;
	}
	
	if(freq>=1) {
		data &= ~0x0001;
		freq -= 1;
	}
	return data;
}

/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/f4/rcc.h>
#include <libopencm3/stm32/f4/gpio.h>
#include <libopencm3/stm32/f4/timer.h>
#include <libopencm3/cm3/nvic.h>

#include "led_disp.h"

const uint32_t PIN_DATA[NUM_MODULES] = {	// this is not very fancy but it'll do...
	PIN_DATA_1,									// TODO: find free pins
	PIN_DATA_2,
	PIN_DATA_3,
	PIN_DATA_4,
	PIN_DATA_5,
};


// // // // // // // // // // 
// LED POSITION IN BLOCK
// // // // // // // // // // 
const uint32_t blockMapping[8][4] = {
	{25, 30,  6,  5},
	{29, 31,  2,  1},
	{27, 28,  0,  3},
	{24, 26,  4,  7},
	{23, 22,  8,  9},
	{21, 19, 11, 10},
	{18, 17, 14, 12},
	{16, 20, 13, 15}
};


void set_pixel_in_block(uint32_t *data, uint32_t x, uint32_t y, bool enable);
void set_pixel_in_module(uint32_t *modulebuffer, uint32_t x, uint32_t y, bool enable);

typedef uint32_t framebuffer_t[NUM_MODULES][MODULEBUFFER_SIZE];

framebuffer_t framebuffer1, framebuffer2;
framebuffer_t *onscreenBuffer	= &framebuffer1;
framebuffer_t *offscreenBuffer	= &framebuffer2;


volatile uint32_t globalFlags = SEND_FRAMEBUFFER;


void led_disp_set_flag(int flag){
	globalFlags |= flag;
}


void led_disp_reset_flag(int flag){
	globalFlags &= ~flag;
}


bool led_disp_get_flag(int flag){
	return globalFlags & flag;
}


void led_disp_init(void)
{
	// // // // // // // // // // 
	// Enable Peripheral Clocks
	// // // // // // // // // // 

	// Port E is needed for more LEDs
	rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPAEN);
	rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPCEN);

	// enable TIM1 clock
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_TIM1EN);


	// // // // // // // // // // 
	// Set Up GPIO Pins
	// // // // // // // // // // 

	gpio_mode_setup(PORT_CLK, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_CLK);
	gpio_mode_setup(PORT_LOAD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LOAD);
	for(int i=0; i< NUM_MODULES; i++){
		gpio_mode_setup(PORT_DATA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_DATA[i]);
	}


	// // // // // // // // // // 
	// Init Timer 1
	// // // // // // // // // // 

	// global interrupt config
	nvic_enable_irq(NVIC_TIM1_UP_TIM10_IRQ);

	// *** TIM1 ***

	// - upcounter
	// - clock: CK_INT
	// - only overflow generates update interrupt
	TIM1_CR1 |= TIM_CR1_URS;

	// defaults for TIM_CR2

	// enable update interrupt
	TIM1_DIER |= TIM_DIER_UIE;

	// prescaler
	TIM1_PSC = 119; // count up by 1 every 1 us

	// auto-reload (maximum value)
	TIM1_ARR = 99; // overflow every 100 us

	// GO!
	TIM1_CR1 |= TIM_CR1_CEN;
}


void set_pixel_in_block(uint32_t *data, uint32_t x, uint32_t y, bool enable)
{
	if(enable) {
		//*data |= (1 << blockMapping[y][x]);
		*data |= (1 << (31 - blockMapping[y][x]));	//TODO: do inversion in blockMapping array
	} else {
		//*data &= ~(1 << blockMapping[y][x]);
		*data &= ~(1 << (31 - blockMapping[y][x]));
	}
}


void set_pixel_in_module(uint32_t *modulebuffer, uint32_t x, uint32_t y, bool enable)
{
	uint32_t blockX = x / 4;
	uint32_t blockY = y / 8;

	uint32_t blockIdx = blockY * 8 + blockX;

	set_pixel_in_block(&(modulebuffer[blockIdx]), x - (4 * blockX), y - (8 * blockY), enable);
}


void led_disp_set_pixel(uint32_t x, uint32_t y, bool enable){
	uint32_t moduleIdx = x/32;
    y = 23-y;
    x = x - (32*moduleIdx);
    x = 31 - x;

	set_pixel_in_module((*offscreenBuffer)[moduleIdx], x, y, enable);
}


void led_disp_clear_buffer(void){
	for(int i=0; i<NUM_MODULES; i++){
		for(int j=0; j<MODULEBUFFER_SIZE; j++){
			(*offscreenBuffer)[i][j] = 0;
		}
	}
}


void led_disp_flip_buffers(void){
	framebuffer_t *swp = onscreenBuffer;
	onscreenBuffer = offscreenBuffer;
	offscreenBuffer = swp;
}


void tim1_up_tim10_isr(void)
{
	static uint32_t tickCount = 0;

	static uint32_t bitIndex = 0;
	static uint32_t blockIndex = 0;

	static bool risingEdge = false;

	// check for update interrupt
	if(TIM1_SR & TIM_SR_UIF) {
		if(globalFlags & SEND_FRAMEBUFFER) {
			gpio_clear(PORT_LOAD, PIN_LOAD);

			if(risingEdge) {
				// rising edge
				gpio_set(PORT_CLK, PIN_CLK);

				bitIndex++;
				if(bitIndex == 33) {
					blockIndex++;

					if(blockIndex == 24) {
						gpio_set(PORT_LOAD, PIN_LOAD);
						blockIndex = 0;

						globalFlags &= ~SEND_FRAMEBUFFER;
					}

					bitIndex = 0;
				}

				risingEdge = false;
			} else {
				// falling edge
				gpio_clear(PORT_CLK, PIN_CLK);

				// update data pin
				if(bitIndex < 32) {
					for(int moduleIdx = 0; moduleIdx < NUM_MODULES; moduleIdx++){
						if(((*onscreenBuffer)[moduleIdx][blockIndex] & (1 << (bitIndex))) != 0) {
							gpio_set(PORT_DATA, PIN_DATA[moduleIdx]);
						} else {
							gpio_clear(PORT_DATA, PIN_DATA[moduleIdx]);
						}
					}
				}

				risingEdge = true;
			}
		}

		tickCount++;

		TIM1_SR &= ~(TIM_SR_UIF); // clear interrupt flag
	}
}

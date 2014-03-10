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

#include <stdlib.h>

#include "led_disp.h"



static void update_modulebuffer(void)
{
	static int phase = 0;

	// clear it all
	//for(int i = 0; i < MODULEBUFFER_SIZE; i++) {
	//	modulebuffer[i] = 0;
	//}

	for(int x = 0; x < 32*5; x++) {
		for(int y = 0; y < 24; y++) {
			led_disp_set_pixel(x, y, (x + y + phase) & 0x1);
		}
	}
	
    led_disp_flip_buffers();

	//set_pixel(rand() % 32, rand() % 24, true);
	//set_pixel(rand() % 32, rand() % 24, false);
	//set_pixel(rand() % 32, rand() % 24, true);
	//set_pixel(rand() % 32, rand() % 24, false);

	phase++;
}


static void init_gpio(void)
{
	// Set up LED outputs for PWM
	gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE,
			GPIO12 | GPIO13 | GPIO14 | GPIO15);

	gpio_set_af(GPIOD, 2, GPIO12 | GPIO13 | GPIO14 | GPIO15);
}


static void init_clock(void)
{
	/* Set STM32 to 120 MHz. */
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);

	// enable GPIO clocks:
	// Port D is needed for LEDs
	rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPDEN);

	// Set up USART2
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);

	RCC_APB2ENR |= RCC_APB2ENR_TIM1EN;

	// enable TIM4 clock
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM4EN);
}


static void init_timer(void)
{
	// *** TIM4 *** //TODO: is this TIM4 stuff necessary?
	timer_reset(TIM4);
	timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	// set up PWM channels
	timer_set_oc_mode(TIM4, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM4, TIM_OC2, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM4, TIM_OC3, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM4, TIM_OC4, TIM_OCM_PWM1);
	timer_enable_oc_output(TIM4, TIM_OC1);
	timer_enable_oc_output(TIM4, TIM_OC2);
	timer_enable_oc_output(TIM4, TIM_OC3);
	timer_enable_oc_output(TIM4, TIM_OC4);
	timer_enable_oc_preload(TIM4, TIM_OC1);
	timer_enable_oc_preload(TIM4, TIM_OC2);
	timer_enable_oc_preload(TIM4, TIM_OC3);
	timer_enable_oc_preload(TIM4, TIM_OC4);

	// prescaler
	timer_set_prescaler(TIM4, 120); // count up by 1 every 1 us

	// auto-reload value
	timer_set_period(TIM4, 1000); // 1000 Hz PWM

	// GO!
	timer_enable_counter(TIM4);
}

int main(void)
{
	init_clock();
	init_gpio();
	init_timer();

	led_disp_init();

	while (1) {
		if(!(led_disp_get_flag(SEND_FRAMEBUFFER))) {
			update_modulebuffer();
			led_disp_set_flag(SEND_FRAMEBUFFER);
		}
	}

	return 0;
}

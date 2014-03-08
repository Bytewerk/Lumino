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

#include <stdio.h>

#define PORT_CLK	 GPIOE
#define PORT_DATA  GPIOE
#define PORT_LOAD  GPIOE

#define PIN_CLK		 GPIO7
#define PIN_DATA	 GPIO8
#define PIN_LOAD	 GPIO9

static void init_gpio(void)
{
	// Set up LED outputs for PWM
	gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE,
			GPIO12 | GPIO13 | GPIO14 | GPIO15);

	gpio_set_af(GPIOD, 2, GPIO12 | GPIO13 | GPIO14 | GPIO15);

	gpio_mode_setup(PORT_CLK, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_CLK);
	gpio_mode_setup(PORT_DATA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_DATA);
	gpio_mode_setup(PORT_LOAD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIN_LOAD);
}

static void init_clock(void)
{
	/* Set STM32 to 120 MHz. */
	rcc_clock_setup_hse_3v3(&hse_8mhz_3v3[CLOCK_3V3_120MHZ]);

	// enable GPIO clocks:
	// Port D is needed for LEDs
	rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPDEN);
	// Port E is needed for more LEDs
	rcc_peripheral_enable_clock(&RCC_AHB1ENR, RCC_AHB1ENR_IOPEEN);

	// enable TIM1 clock
	rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_TIM1EN);

	// Set up USART2
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2);
	gpio_set_af(GPIOA, GPIO_AF7, GPIO2);

	RCC_APB2ENR |= RCC_APB2ENR_TIM1EN;

	// enable TIM4 clock
	rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM4EN);
}

static void init_timer(void)
{
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

	// 48 kHz interrupt frequency
	// TIM1_PSC = 24; // count up by 1 every 208.33 ns
	// TIM1_ARR = 99; // multiply interval by 100 -> 20.833 us

	// GO!
	TIM1_CR1 |= TIM_CR1_CEN;

	// *** TIM4 ***
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

	gpio_set(PORT_LOAD, PIN_LOAD);

	while (1) {
	}

	return 0;
}

void tim1_up_tim10_isr(void)
{
	static uint32_t tickCount = 0;
	static uint32_t bitIndex = 0;
	static uint32_t delay = 10;
	static bool risingEdge = false;

	// check for update interrupt
	if(TIM1_SR & TIM_SR_UIF) {
		if((tickCount % delay) == 0) {
			if(risingEdge) {
				// rising edge
				gpio_set(PORT_CLK, PIN_CLK);
				bitIndex++;
				risingEdge = false;
			} else {
				// falling edge
				gpio_clear(PORT_CLK, PIN_CLK);
				if(bitIndex >= 24*33-1) {
					if((bitIndex % 33) == 0) {
						gpio_set(PORT_DATA, PIN_DATA);
					} else {
						gpio_clear(PORT_DATA, PIN_DATA);
					}
				} else {
					gpio_clear(PORT_DATA, PIN_DATA);
				}

				risingEdge = true;
			}

			if(bitIndex < 24*33) {
				delay = 10;
			} else {
				delay = 25000;
			}
		}

		tickCount++;

		TIM1_SR &= ~(TIM_SR_UIF); // clear interrupt flag
	}
}

void hard_fault_handler(void)
{
	while (1);
}

void usage_fault_handler(void)
{
	while (1);
}

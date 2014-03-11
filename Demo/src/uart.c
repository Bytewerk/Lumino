/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 * Copyright (C) 2011 Stephen Caudle <scaudle@doceme.com>
 * Copyright (C) 2013 Piotr Esden-Tempski <piotr@esden.net>
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

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#include "uart.h"
#include "led_disp.h"


void uart_init(void)
{
	/* Enable GPIOD clock for USARTs. */
	rcc_periph_clock_enable(RCC_GPIOA);

	/* Enable clocks for USART4. */
	rcc_periph_clock_enable(RCC_UART4);

	/* Enable the USART4 interrupt. */
	nvic_enable_irq(NVIC_UART4_IRQ);

	/* Setup GPIO pins for USART4 transmit. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO0);

	/* Setup GPIO pins for USART4 receive. */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO1);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_25MHZ, GPIO1);

	/* Setup USART4 TX and RX pin as alternate function. */
	gpio_set_af(GPIOA, GPIO_AF8, GPIO0);
	gpio_set_af(GPIOA, GPIO_AF8, GPIO1);

	/* Setup USART4 parameters. */
	usart_set_baudrate(UART4, 38400);
	usart_set_databits(UART4, 8);
	usart_set_stopbits(UART4, USART_STOPBITS_1);
	usart_set_mode(UART4, USART_MODE_TX_RX);
	usart_set_parity(UART4, USART_PARITY_NONE);
	usart_set_flow_control(UART4, USART_FLOWCONTROL_NONE);

	/* Enable USART4 Receive interrupt. */
	usart_enable_rx_interrupt(UART4);

	/* Finally enable the USART. */
	usart_enable(UART4);
}


static void handle_data( uint8_t data, uint32_t *pos ){
    uint32_t oldpos = *pos;
    for( ; *pos-oldpos < 8; (*pos)++ ){
        led_disp_set_pixel( *pos % NUM_MODULES*32+1, *pos / (NUM_MODULES*32), data & 0x80);
        data <<= 1;
    }
}



void uart4_isr( void )
{
	uint8_t data = UART_ESC;
    static bool esc = false;
    static bool in_trans = false;
    static uint32_t pos = 0;

	/* Check if we were called because of RXNE. */
	if (((USART_CR1(UART4) & USART_CR1_RXNEIE) != 0) &&
	    ((USART_SR(UART4) & USART_SR_RXNE) != 0)) {

		/* Retrieve the data from the peripheral. */
		data = usart_recv(UART4);

        if( esc == true ){
            esc = false;
            switch( data ){
                case UART_ESC:
                    if( in_trans == true ){
                        handle_data( data, &pos );
                    }else{
                        // error
                    }
                    break;

                case UART_START:
                    if( in_trans == false ){
                        in_trans = true;
                        pos = 0;
                    }else{
                        // error
                    } 
                    break;

                case UART_STOP:
                    if( in_trans == true ){
                        in_trans = false;
                        led_disp_flip_buffers();
		                led_disp_set_flag(SEND_FRAMEBUFFER);
                    }else{
                        // error
                    }
                    break;

                default:
                    // error
                    break;
            }
        }else{
            switch( data ){
                case UART_ESC:
                    esc = true;
                    break;

                default:
                    if( in_trans == true ){
                        handle_data( data, &pos );
                    }else{
                        // error
                    }
                    break;
            }
        }
	}

	/* Check if we were called because of TXE. */
	if (((USART_CR1(UART4) & USART_CR1_TXEIE) != 0) &&
	    ((USART_SR(UART4) & USART_SR_TXE) != 0)) {

		///* Put data into the transmit register. */
		//usart_send(USART2, data);

		///* Disable the TXE interrupt as we don't need it anymore. */
		//usart_disable_tx_interrupt(USART2);
	}
}

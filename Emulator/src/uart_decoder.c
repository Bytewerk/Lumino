// vim: ts=2 sw=2 noexpandtab

#include <stdint.h>
#include <stdio.h>

#include "display.h"

#include "uart_decoder.h"

#define ESC   0x1B
#define START 0x02
#define END   0x03

uint8_t  framebuffer[DISPLAY_DATA_LEN];
uint32_t index = 0;
int      special = 0;

uint8_t* uart_decode( uint8_t data )
{
	// prevent writing outside of buffer
	if(index >= DISPLAY_DATA_LEN) {
		index = 0;
	}

	if(special) {
		switch(data) {
			case START:
				printf("START\n");
				index = 0;
				break;

			case END:
				printf("END\n");
				if(index == 0) { // set by overflow protection on correct frames
					special = 0;
					return framebuffer;
				}
				break;

			case ESC:
				printf("ESC\n");
				framebuffer[index] = ESC;
				index++;
				break;

			default:
				printf("UNKNOWN\n");
				break;
		}

		special = 0;
	} else {
		if(data == ESC) {
			printf("Escape sequence at %d: ", index);
			special = 1;
		} else {
			framebuffer[index] = data;
			index++;
		}
	}

	return NULL;
}

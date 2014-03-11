// vim: ts=2 sw=2 noexpandtab

#include <stdint.h>

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
				index = 0;
				break;

			case END:
				if(index == 0) { // set by overflow protection on correct frames
					return framebuffer;
				}
				break;

			case ESC:
				framebuffer[index] = ESC;
				index++;
				break;
		}

		special = 0;
	} else {
		if(data == ESC) {
			special = 1;
		} else {
			framebuffer[index] = data;
			index++;
		}
	}

	return NULL;
}

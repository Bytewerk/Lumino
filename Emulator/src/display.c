// vim: ts=2 sw=2 noexpandtab

#include <SDL/SDL.h>

#include "display.h"

#define SCALE 4
#define SCREEN_WIDTH	(DISPLAY_WIDTH * SCALE)
#define SCREEN_HEIGHT (DISPLAY_HEIGHT * SCALE)

#define IS_BLACK(c) (c == 0)

SDL_Surface *screen;

void display_init( void )
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if((screen = SDL_SetVideoMode(DISPLAY_WIDTH*SCALE, DISPLAY_HEIGHT*SCALE, 24, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
		printf("Could not set SDL video mode: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_WM_SetCaption("LED-Display", "LED-Display");
}

void display_update( uint8_t *data )
{
	uint32_t x = 0, y = 0;

	while(y < DISPLAY_HEIGHT) {
		uint32_t bit  = (y * DISPLAY_WIDTH + x);
		uint32_t byte = bit / 8;
		bit %= 8;

		SDL_Rect pixRect = {SCALE*x, SCALE*y, SCALE, SCALE};

		if(data[byte] & (0x80 >> bit)) {
			SDL_FillRect(screen, &pixRect, 0xFFFF0000);
		} else {
			SDL_FillRect(screen, &pixRect, 0xFF000000);
		}

		x++;
		if(x == DISPLAY_WIDTH) {
			x = 0;
			y++;
		}
	}

	SDL_Flip(screen);
}

void display_stop( void )
{
	SDL_Quit();
}

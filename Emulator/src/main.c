// vim: ts=2 sw=2 noexpandtab

#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <SDL/SDL.h>

#include "display.h"
#include "uart_decoder.h"

int main(void)
{
	int running = 1;
	uint8_t *ptr = NULL;
	SDL_Event event;

	fd_set rfds;
	struct timeval tv;
	int retval;

	uint8_t c;

	// initialize modules
	display_init();

	// main loop
	while(running) {
		FD_ZERO(&rfds);
		FD_SET(0, &rfds);

		tv.tv_sec = 0;
		tv.tv_usec = 100000;

		retval = select(1, &rfds, NULL, NULL, &tv);

		if(retval && FD_ISSET(0, &rfds)) { // data available
			read(0, &c, 1);

			if( (ptr = uart_decode(c)) ) {
				display_update(ptr);
			}
		}

		while(SDL_PollEvent(&event)) {
			if(event.type == SDL_QUIT) {
				running = 0;
				break;
			}

			if(event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
					case SDLK_ESCAPE:
						running = 0;
						break;

					default:
						// do nothing
						break;
				}
			}
		}
	}
}

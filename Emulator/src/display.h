// vim: ts=2 sw=2 noexpandtab

#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdlib.h>

#define DISPLAY_HEIGHT  24
#define DISPLAY_WIDTH  160

#define DISPLAY_DATA_LEN (DISPLAY_HEIGHT*DISPLAY_WIDTH/8)

void display_init( void );

void display_update( uint8_t *data );

void display_stop( void );

#endif // DISPLAY_H

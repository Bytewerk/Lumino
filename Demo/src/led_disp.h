#ifndef __LED_DISP_H__
#define __LED_DISP_H__



// // // // // // // // // // 
// AMOUNT OF LEDs / MODULES
// // // // // // // // // // 
#define MODULEBUFFER_SIZE    24
#define NUM_MODULES 5



// // // // // // // // // // 
// PORTS AND PINS
// // // // // // // // // // 
#define PORT_CLK	 GPIOE
#define PORT_DATA	 GPIOE
#define PORT_LOAD	 GPIOE

#define PIN_CLK		 GPIO7
#define PIN_LOAD	 GPIO9

#define PIN_DATA_1	GPIO6
#define PIN_DATA_2	GPIO7
#define PIN_DATA_3	GPIO8
#define PIN_DATA_4	GPIO9
#define PIN_DATA_5	GPIO10



// // // // // // // // // // 
// FLAGS
// // // // // // // // // // 
#define SEND_FRAMEBUFFER (1 << 0)



// // // // // // // // // // 
// FUNCTIONS
// // // // // // // // // // 
void led_disp_init(void);

void led_disp_set_pixel(uint32_t x, uint32_t y, bool enable);

void led_disp_clear_buffer(void);

void led_disp_flip_buffers(void);

void led_disp_set_flag(int flag);

void led_disp_reset_flag(int flag);

bool led_disp_get_flag(int flag);




#endif // __LED_DISP_H__

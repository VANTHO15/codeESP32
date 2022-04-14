#ifndef __WS2812B_H
#define __WS2812B_H

void ws2812_init(int tx_pin, int number);
void ws2812_set_rgb(int index, int r, int g, int b);
void ws2812_set_off(void);
void ws2812_set_all_rgb(int r, int g, int b);
#endif
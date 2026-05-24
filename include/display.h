#ifndef DISPLAY_H
#define DISPLAY_H
 
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <stdint.h>
 
/*
 * Pini TFT ST7735S — Arduino Uno, hardware SPI:
 *   PB0 (D8)  = DC
 *   PB1 (D9)  = RST
 *   PB2 (D10) = CS
 *   PB3 (D11) = MOSI  ← hardware SPI
 *   PB5 (D13) = SCK   ← hardware SPI
 *
 * Rotatie = 1 (landscape): 160 x 128 pixeli
 */
#define TFT_CS   10
#define TFT_RST   9
#define TFT_DC    8
 
/* Simboluri role */
#define SYM_7        0
#define SYM_BAR      1
#define SYM_STAR     2
#define SYM_CHERRY   3
#define SYM_DIAMOND  4
#define NUM_SYMBOLS  5
 
/* Culori extra */
#define COLOR_GRAY   0x8410
 
void display_init(void);
void display_clear(void);
 
void display_show_start(void);
void display_show_reels(uint8_t s1, uint8_t s2, uint8_t s3);
void display_show_spinning(void);
void display_show_credits(uint16_t credits, uint8_t bet);
 
void display_show_win(uint16_t amount);
void display_show_lose(void);
void display_show_jackpot(void);
void display_show_redblack(void);
void display_show_gameover(void);
void display_show_stats(uint16_t spins, uint16_t best);
 
#endif /* DISPLAY_H */
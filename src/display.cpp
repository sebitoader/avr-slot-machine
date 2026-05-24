#include "display.h"
#include <stdlib.h>

static Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

/*
 * Layout ecran landscape (160 x 128):
 *
 *  y=0  ┌──────────────────────────────┐
 *       │     Bara titlu  (18px)       │
 *  y=18 ├──────────────────────────────┤
 *       │   Role  (60px, y=20..79)     │
 *       │  [ROLA0] [ROLA1] [ROLA2]     │
 *  y=80 ├──────────────────────────────┤
 *       │   Zona status  (48px)        │
 * y=128 └──────────────────────────────┘
 *
 * Rolele (46 latime x 60 inaltime):
 *   Rola 0: x=4,   Rola 1: x=57,   Rola 2: x=110
 *   Gap intre role: 7px — Total: 4+46+7+46+7+46+4 = 160 ✓
 */

/* ───────────────────────────────────────────────────────────────
 * Culori
 * ─────────────────────────────────────────────────────────────── */
#define COLOR_GOLD       0xFEA0
#define COLOR_SILVER     0xDEFB
#define COLOR_DARKGRAY   0x2104
#define COLOR_MEDGRAY    0x4208
#define COLOR_PANEL      0x1082   /* fundal role */
#define COLOR_BORDER     0x8410
#define COLOR_DARKRED    0x8000
#define COLOR_DARKBLUE   0x000F
#define COLOR_DARKGREEN  0x0320
#define COLOR_DARKCYAN   0x0208

/* ───────────────────────────────────────────────────────────────
 * Coordonate role
 * ─────────────────────────────────────────────────────────────── */
static const uint8_t REEL_X[3] = { 4, 57, 110 };
#define REEL_Y   20
#define REEL_W   46
#define REEL_H   60

/* ───────────────────────────────────────────────────────────────
 * Culori bordura simboluri (pentru chenar rola)
 * ─────────────────────────────────────────────────────────────── */
static const uint16_t sym_border_color[] = {
    COLOR_GOLD,        /* 7      */
    ST77XX_WHITE,      /* BAR    */
    ST77XX_YELLOW,     /* STAR   */
    ST77XX_RED,        /* CHERRY */
    ST77XX_CYAN        /* DIAMOND*/
};

/* ───────────────────────────────────────────────────────────────
 * Desenare simboluri grafice
 * Toate coordonatele relative la (x, y) = coltul stanga-sus al rolei
 * ─────────────────────────────────────────────────────────────── */

/* SYM_7: cifra 7 mare in gold/red */
static void draw_seven(uint8_t x, uint8_t y)
{
    /* bara orizontala sus */
    tft.fillRect(x + 5,  y + 6,  34, 9, COLOR_GOLD);
    tft.fillRect(x + 5,  y + 6,  34, 3, ST77XX_RED);   /* accent rosu sus */
    /* diagonala dreapta-jos (trepte) */
    tft.fillRect(x + 27, y + 13, 12, 8, COLOR_GOLD);
    tft.fillRect(x + 20, y + 20, 12, 8, COLOR_GOLD);
    tft.fillRect(x + 13, y + 28, 12, 8, COLOR_GOLD);
    tft.fillRect(x + 7,  y + 36, 10, 12, COLOR_GOLD);
    /* highlight (linie luminoasa) */
    tft.drawFastHLine(x + 6, y + 7, 32, ST77XX_WHITE);
}

/* SYM_BAR: trei bare argintii */
static void draw_bar(uint8_t x, uint8_t y)
{
    uint16_t c1 = COLOR_SILVER;
    uint16_t c2 = ST77XX_WHITE;
    uint16_t cd = COLOR_DARKGRAY;

    /* Bara sus */
    tft.fillRoundRect(x + 4,  y + 5,  36, 13, 3, c1);
    tft.drawFastHLine(x + 5,  y + 6,  34, c2);        /* shine */
    tft.drawRoundRect(x + 4,  y + 5,  36, 13, 3, cd);

    /* Bara mijloc */
    tft.fillRoundRect(x + 4,  y + 22, 36, 13, 3, c1);
    tft.drawFastHLine(x + 5,  y + 23, 34, c2);
    tft.drawRoundRect(x + 4,  y + 22, 36, 13, 3, cd);

    /* Bara jos */
    tft.fillRoundRect(x + 4,  y + 39, 36, 13, 3, c1);
    tft.drawFastHLine(x + 5,  y + 40, 34, c2);
    tft.drawRoundRect(x + 4,  y + 39, 36, 13, 3, cd);
}

/* SYM_STAR: stea cu 4 varfuri + 4 triunghiuri diagonale */
static void draw_star(uint8_t x, uint8_t y)
{
    int16_t cx = x + 23, cy = y + 30;
    int16_t R = 19, r = 7;

    /* 4 triunghiuri axiale */
    tft.fillTriangle(cx - r, cy - r, cx + r, cy - r, cx,     cy - R, ST77XX_YELLOW);
    tft.fillTriangle(cx - r, cy + r, cx + r, cy + r, cx,     cy + R, ST77XX_YELLOW);
    tft.fillTriangle(cx - r, cy - r, cx - r, cy + r, cx - R, cy,     ST77XX_YELLOW);
    tft.fillTriangle(cx + r, cy - r, cx + r, cy + r, cx + R, cy,     ST77XX_YELLOW);
    /* patrat central */
    tft.fillRect(cx - r, cy - r, r * 2, r * 2, ST77XX_YELLOW);
    /* 4 triunghiuri diagonale (umplere colturi) */
    tft.fillTriangle(cx - r, cy - r, cx + r, cy - r, cx - R + 4, cy - R + 4, ST77XX_YELLOW);
    tft.fillTriangle(cx + r, cy - r, cx + r, cy + r, cx + R - 4, cy - R + 4, ST77XX_YELLOW);
    tft.fillTriangle(cx - r, cy + r, cx + r, cy + r, cx - R + 4, cy + R - 4, ST77XX_YELLOW);
    tft.fillTriangle(cx - r, cy - r, cx - r, cy + r, cx - R + 4, cy + R - 4, ST77XX_YELLOW);
    /* shine */
    tft.fillCircle(cx - 5, cy - 10, 3, ST77XX_WHITE);
}

/* SYM_CHERRY: doua cirese rosii cu codite verzi */
static void draw_cherry(uint8_t x, uint8_t y)
{
    int16_t lx = x + 14, ly = y + 38; /* centru cireasa stanga */
    int16_t rx = x + 32, ry = y + 38; /* centru cireasa dreapta */
    int16_t jx = x + 23, jy = y + 18; /* junctiune codite */

    /* Codite */
    for (int8_t i = -1; i <= 1; i++) {
        tft.drawLine(lx + i, ly - 8, jx + i, jy, 0x0700);   /* verde inchis */
        tft.drawLine(rx + i, ry - 8, jx + i, jy, 0x0700);
    }
    tft.drawLine(jx, jy, jx, y + 10, 0x07E0);                /* coada sus */
    /* umbre codite */
    tft.drawLine(lx,     ly - 8, jx,     jy, 0x07E0);
    tft.drawLine(rx,     ry - 8, jx,     jy, 0x07E0);

    /* Cirese (umbre mai inchise sub) */
    tft.fillCircle(lx, ly + 1, 9, COLOR_DARKRED);
    tft.fillCircle(rx, ry + 1, 9, COLOR_DARKRED);
    /* Cirese (rosu viu) */
    tft.fillCircle(lx, ly, 9, ST77XX_RED);
    tft.fillCircle(rx, ry, 9, ST77XX_RED);
    /* Shine */
    tft.fillCircle(lx - 3, ly - 3, 2, ST77XX_WHITE);
    tft.fillCircle(rx - 3, ry - 3, 2, ST77XX_WHITE);
}

/* SYM_DIAMOND: diamant cu shine */
static void draw_diamond(uint8_t x, uint8_t y)
{
    int16_t cx = x + 23, cy = y + 32;
    int16_t hw = 17, hh = 22; /* jumatate latime/inaltime */

    /* Jumatatea de sus (cyan deschis) */
    tft.fillTriangle(cx,      cy - hh,
                     cx - hw, cy,
                     cx + hw, cy,      ST77XX_CYAN);
    /* Jumatatea de jos (cyan mai inchis) */
    tft.fillTriangle(cx,      cy + hh,
                     cx - hw, cy,
                     cx + hw, cy,      COLOR_DARKCYAN);
    /* Linie de mijloc */
    tft.drawFastHLine(cx - hw, cy, hw * 2, ST77XX_WHITE);
    /* Shine: triunghi alb mic in coltul stanga-sus */
    tft.fillTriangle(cx,      cy - hh,
                     cx - 6,  cy - 8,
                     cx + 4,  cy - 14, ST77XX_WHITE);
    /* Contur */
    tft.drawTriangle(cx,      cy - hh,
                     cx - hw, cy,
                     cx + hw, cy,      ST77XX_WHITE);
    tft.drawTriangle(cx,      cy + hh,
                     cx - hw, cy,
                     cx + hw, cy,      ST77XX_WHITE);
}

/* ───────────────────────────────────────────────────────────────
 * Deseneaza o rola completa (fundal + chenar + simbol)
 * ─────────────────────────────────────────────────────────────── */
static void draw_reel(uint8_t col, uint8_t sym)
{
    uint8_t x = REEL_X[col];
    uint8_t y = REEL_Y;
    uint16_t bc = sym_border_color[sym];

    /* Fundal rola */
    tft.fillRect(x, y, REEL_W, REEL_H, COLOR_PANEL);

    /* Chenar dublu colorat */
    tft.drawRect(x,     y,     REEL_W,   REEL_H,   bc);
    tft.drawRect(x + 1, y + 1, REEL_W-2, REEL_H-2, COLOR_BORDER);

    /* Simbol grafic */
    switch (sym) {
        case SYM_7:       draw_seven (x, y); break;
        case SYM_BAR:     draw_bar   (x, y); break;
        case SYM_STAR:    draw_star  (x, y); break;
        case SYM_CHERRY:  draw_cherry(x, y); break;
        case SYM_DIAMOND: draw_diamond(x, y); break;
        default: break;
    }
}

/* ───────────────────────────────────────────────────────────────
 * Bara de titlu fixa
 * ─────────────────────────────────────────────────────────────── */
static void draw_title_bar(void)
{
    tft.fillRect(0, 0, 160, 18, 0x000F);               /* fond albastru inchis */
    tft.drawFastHLine(0, 17, 160, COLOR_GOLD);

    tft.setTextColor(COLOR_GOLD);
    tft.setTextSize(1);
    tft.setCursor(4, 5);
    tft.print(F("* AVR SLOT MACHINE *"));
}

/* ───────────────────────────────────────────────────────────────
 * Bara de status (zona de jos y=81..128)
 * — curata zona cu fillRect, text fara background
 * ─────────────────────────────────────────────────────────────── */
static void clear_status(void)
{
    tft.fillRect(0, 81, 160, 47, ST77XX_BLACK);
    tft.drawFastHLine(0, 81, 160, COLOR_BORDER);
}

/* ───────────────────────────────────────────────────────────────
 * API public
 * ─────────────────────────────────────────────────────────────── */

void display_init(void)
{
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(1);                  /* landscape: 160 x 128 */
    tft.fillScreen(ST77XX_BLACK);
}

void display_clear(void)
{
    tft.fillScreen(ST77XX_BLACK);
}

/* Afiseaza cele 3 role cu simbolurile finale */
void display_show_reels(uint8_t s1, uint8_t s2, uint8_t s3)
{
    draw_reel(0, s1);
    draw_reel(1, s2);
    draw_reel(2, s3);
}

/* Un frame de spinning: 3 simboluri aleatoare */
void display_show_spinning(void)
{
    draw_reel(0, (uint8_t)(rand() % NUM_SYMBOLS));
    draw_reel(1, (uint8_t)(rand() % NUM_SYMBOLS));
    draw_reel(2, (uint8_t)(rand() % NUM_SYMBOLS));
}

/* Bara credite + miza */
void display_show_credits(uint16_t credits, uint8_t bet)
{
    clear_status();

    /* Linie credite */
    tft.setTextSize(1);
    tft.setCursor(4, 88);
    tft.setTextColor(COLOR_MEDGRAY);
    tft.print(F("CREDITE:"));
    tft.setTextColor(ST77XX_GREEN);
    tft.print(credits);

    tft.setCursor(100, 88);
    tft.setTextColor(COLOR_MEDGRAY);
    tft.print(F("MIZA:"));
    tft.setTextColor(ST77XX_YELLOW);
    tft.print(bet);

    /* Separator */
    tft.drawFastHLine(0, 100, 160, COLOR_DARKGRAY);

    /* Hint butoane */
    tft.setCursor(4, 105);
    tft.setTextColor(0x4208);
    tft.print(F("[SPIN]Roteste  [BET+]Miza"));
}

/* Ecran de start */
void display_show_start(void)
{
    tft.fillScreen(ST77XX_BLACK);
    draw_title_bar();

    /* Logo central */
    tft.setTextSize(2);
    tft.setTextColor(COLOR_GOLD);
    tft.setCursor(14, 28);
    tft.print(F("AVR SLOTS"));

    /* Sublinie aurie */
    tft.drawFastHLine(14, 47, 130, COLOR_GOLD);

    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(32, 58);
    tft.print(F("Apasa  SPIN"));
    tft.setCursor(28, 71);
    tft.print(F("pentru  START!"));

    tft.setTextColor(COLOR_MEDGRAY);
    tft.setCursor(6, 88);
    tft.print(F("Tine BLACK 2s -> Stats"));
}

/* Castig */
void display_show_win(uint16_t amount)
{
    clear_status();

    tft.setTextSize(1);
    tft.setCursor(4, 88);
    tft.setTextColor(ST77XX_GREEN);
    tft.print(F("CASTIG: "));
    tft.setTextColor(ST77XX_YELLOW);
    tft.print(F("+"));
    tft.print(amount);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(F(" CR!"));

    tft.drawFastHLine(0, 100, 160, COLOR_DARKGREEN);

    tft.setCursor(4, 106);
    tft.setTextColor(0x4208);
    tft.print(F("[BET]Incaseaza [SPIN]Dublu"));
}

/* Pierdere */
void display_show_lose(void)
{
    clear_status();

    tft.setTextSize(1);
    tft.setCursor(4, 88);
    tft.setTextColor(ST77XX_RED);
    tft.print(F("PIERDUT!"));
    tft.setTextColor(COLOR_MEDGRAY);
    tft.print(F("  Mai incearca..."));
}

/* Jackpot */
void display_show_jackpot(void)
{
    /* Flash spectaculos */
    for (uint8_t i = 0; i < 4; i++) {
        tft.fillScreen(COLOR_GOLD);
        delay(70);
        tft.fillScreen(ST77XX_BLACK);
        delay(70);
    }

    draw_title_bar();

    tft.setTextSize(3);
    tft.setTextColor(COLOR_GOLD);
    tft.setCursor(4, 26);
    tft.print(F("JACKPOT"));

    tft.drawFastHLine(4,  56, 152, COLOR_GOLD);

    tft.setTextSize(1);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(24, 66);
    tft.print(F("7 7 7  =  x20 !"));

    tft.setTextColor(COLOR_MEDGRAY);
    tft.setCursor(28, 82);
    tft.print(F("Felicitari!"));
}

/* Mini-joc Red or Black */
void display_show_redblack(void)
{
    clear_status();

    tft.setTextSize(1);
    tft.setCursor(4, 88);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(F("Dublu sau nimic?"));

    tft.drawFastHLine(0, 100, 160, COLOR_BORDER);

    /* Doua zone colorate */
    tft.fillRect(0,  101, 78, 26, 0x1800);    /* fond rosu inchis */
    tft.fillRect(82, 101, 78, 26, 0x0180);    /* fond verde inchis */
    tft.drawFastVLine(80, 101, 26, ST77XX_WHITE);

    tft.setTextColor(ST77XX_RED);
    tft.setCursor(14, 108);
    tft.print(F("[RED]=Rosu"));

    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(86, 108);
    tft.print(F("[BLK]=Negru"));
}

/* Game over */
void display_show_gameover(void)
{
    tft.fillScreen(ST77XX_BLACK);
    draw_title_bar();

    tft.setTextSize(2);
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(8, 28);
    tft.print(F("GAME OVER"));

    tft.drawFastHLine(8, 49, 144, ST77XX_RED);

    tft.setTextSize(1);
    tft.setTextColor(COLOR_MEDGRAY);
    tft.setCursor(18, 59);
    tft.print(F("Creditele s-au"));
    tft.setCursor(18, 72);
    tft.print(F("terminat!"));

    tft.setTextColor(ST77XX_YELLOW);
    tft.setCursor(4, 90);
    tft.print(F("[SPIN] = Restart cu 100 CR"));
}

/* Statistici */
void display_show_stats(uint16_t spins, uint16_t best)
{
    tft.fillScreen(ST77XX_BLACK);
    draw_title_bar();

    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(1);
    tft.setCursor(40, 24);
    tft.print(F("STATISTICI"));
    tft.drawFastHLine(4, 34, 152, COLOR_GOLD);

    tft.setTextColor(COLOR_MEDGRAY);
    tft.setCursor(8, 44);
    tft.print(F("Total rotiri:"));
    tft.setTextColor(ST77XX_CYAN);
    tft.print(F("  "));
    tft.print(spins);

    tft.setTextColor(COLOR_MEDGRAY);
    tft.setCursor(8, 60);
    tft.print(F("Cel mai mare castig:"));
    tft.setTextColor(ST77XX_GREEN);
    tft.setCursor(8, 73);
    tft.print(best);
    tft.print(F(" credite"));

    tft.drawFastHLine(4, 88, 152, COLOR_BORDER);
    tft.setTextColor(0x4208);
    tft.setCursor(16, 96);
    tft.print(F("[SPIN] = Inapoi la joc"));
}
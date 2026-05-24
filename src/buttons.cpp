#include "buttons.h"
#include <Arduino.h>

typedef struct {
    uint8_t  pin;
    uint8_t  last_raw;
    uint8_t  stable;
    uint32_t changed_at;
    uint8_t  flag;
} Button;

static Button btns[4] = {
    { BTN_SPIN_PIN,  1, 1, 0, 0 },
    { BTN_BET_PIN,   1, 1, 0, 0 },
    { BTN_RED_PIN,   1, 1, 0, 0 },
    { BTN_BLACK_PIN, 1, 1, 0, 0 },
};

void buttons_init(void)
{
    DDRD  &= ~((1 << BTN_SPIN_PIN) | (1 << BTN_BET_PIN) |
               (1 << BTN_RED_PIN)  | (1 << BTN_BLACK_PIN));
    PORTD |=  ((1 << BTN_SPIN_PIN) | (1 << BTN_BET_PIN) |
               (1 << BTN_RED_PIN)  | (1 << BTN_BLACK_PIN));
}

void buttons_update(void)
{
    uint32_t now = millis();
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t raw = !(PIND & (1 << btns[i].pin));
        if (raw != btns[i].last_raw) {
            btns[i].last_raw   = raw;
            btns[i].changed_at = now;
        }
        if ((now - btns[i].changed_at) >= DEBOUNCE_MS) {
            if (raw && !btns[i].stable) btns[i].flag = 1;
            btns[i].stable = raw;
        }
    }
}

static uint8_t consume(uint8_t i)
{
    if (btns[i].flag) { btns[i].flag = 0; return 1; }
    return 0;
}

uint8_t btn_spin_just_pressed(void)  { return consume(0); }
uint8_t btn_bet_just_pressed(void)   { return consume(1); }
uint8_t btn_red_just_pressed(void)   { return consume(2); }
uint8_t btn_black_just_pressed(void) { return consume(3); }

/*
 * FIX: varianta anterioara folosea !(PIND & mask) care e gresit
 * pentru multi-bit. Corect: orice bit din mask e 0 in PIND = buton apasat.
 * ~PIND inverseaza: bit 1 = pin LOW = buton apasat.
 */
uint8_t btn_any_held(void)
{
    uint8_t mask = (1 << BTN_SPIN_PIN) | (1 << BTN_BET_PIN) |
                   (1 << BTN_RED_PIN)  | (1 << BTN_BLACK_PIN);
    return ((~PIND) & mask) != 0;
}
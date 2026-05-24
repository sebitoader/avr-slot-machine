#ifndef BUTTONS_H
#define BUTTONS_H

#include <avr/io.h>
#include <stdint.h>

/*
 * Butoane arcade pe PORTD, INPUT_PULLUP.
 * Stare activa = LOW (butonul scurtcircuiteaza pinul la GND).
 *
 *   PD2 (D2) = SPIN
 *   PD3 (D3) = BET+
 *   PD4 (D4) = RED
 *   PD5 (D5) = BLACK
 */
#define BTN_SPIN_PIN   PD2
#define BTN_BET_PIN    PD3
#define BTN_RED_PIN    PD4
#define BTN_BLACK_PIN  PD5

#define DEBOUNCE_MS    40U

/* Initializeaza pinii ca INPUT_PULLUP */
void buttons_init(void);

/*
 * Apeleaza din loop() sau din game_run().
 * Citeste pinii si actualizeaza flagurile de edge (falling edge = apasare).
 */
void buttons_update(void);

/*
 * Returneaza 1 daca butonul a fost JUST apasat (edge unic),
 * si reseteaza flagul intern. Apeleaza o singura data per eveniment.
 */
uint8_t btn_spin_just_pressed(void);
uint8_t btn_bet_just_pressed(void);
uint8_t btn_red_just_pressed(void);
uint8_t btn_black_just_pressed(void);

/* Stare instantanee (fara debounce) — pentru asteptare eliberare */
uint8_t btn_any_held(void);

#endif /* BUTTONS_H */
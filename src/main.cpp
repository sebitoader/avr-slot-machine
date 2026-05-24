#include <Arduino.h>
#include "buttons.h"
#include "display.h"
#include "dfplayer.h"
#include "game.h"

/*
 * AVR Slot Machine — Arduino Uno R3
 * ──────────────────────────────────────────────────────────────
 * Componente:
 *   TFT ST7735S   — hardware SPI (D8/D9/D10/D11/D13)
 *   DFPlayer Mini — SoftwareSerial (D6=TX, D7=RX)
 *   4x Butoane    — PORTD input pull-up (D2/D3/D4/D5)
 *
 * Structura fisiere:
 *   buttons.h/cpp  — citire butoane cu debounce non-blocant
 *   dfplayer.h/cpp — control modul audio DFPlayer Mini
 *   display.h/cpp  — driver TFT ST7735S (Adafruit)
 *   game.h/cpp     — logica slot machine (masina de stari)
 */

void setup()
{
    Serial.begin(9600);
    Serial.println(F("=== AVR Slot Machine Boot ==="));

    buttons_init();
    Serial.println(F("[OK] Buttons"));

    display_init();
    Serial.println(F("[OK] Display"));

    dfplayer_init();
    /* dfplayer_init afiseaza OK sau FAIL singur */

    game_init();
    Serial.println(F("[OK] Game"));
    Serial.println(F("=== Ready ==="));
}

void loop()
{
    game_run();
}
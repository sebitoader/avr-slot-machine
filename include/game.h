#ifndef GAME_H
#define GAME_H

#include <stdint.h>
#include <avr/eeprom.h>

/* ---------------------------------------------------------------
 * Adrese EEPROM (fiecare uint16_t ocupa 2 bytes)
 * --------------------------------------------------------------- */
#define EEPROM_ADDR_CREDITS   ((uint16_t*)0)
#define EEPROM_ADDR_SPINS     ((uint16_t*)2)
#define EEPROM_ADDR_BEST      ((uint16_t*)4)
#define EEPROM_MAGIC_VAL      0xABCD
#define EEPROM_ADDR_MAGIC     ((uint16_t*)6)

/* ---------------------------------------------------------------
 * Configuratie joc
 * --------------------------------------------------------------- */
#define STARTING_CREDITS   100
#define MIN_BET              1
#define MAX_BET             10

/* Numar de frame-uri si interval animatie spinning */
#define SPIN_FRAMES         18
#define SPIN_FRAME_MS       55

/* ---------------------------------------------------------------
 * Stari masina de stari
 * --------------------------------------------------------------- */
typedef enum {
    STATE_START,        /* Ecran initial, asteapta SPIN */
    STATE_IDLE,         /* Joc activ, asteapta SPIN sau BET+ */
    STATE_SPINNING,     /* Animatie de spinning */
    STATE_RESULT,       /* Afiseaza rezultat, asteapta BET(incaseaza) sau SPIN(dublu) */
    STATE_REDBLACK,     /* Mini-joc: alege ROSU sau NEGRU */
    STATE_GAMEOVER,     /* Credite = 0 */
    STATE_STATS         /* Ecran statistici */
} GameState;

void game_init(void);
void game_run(void);

#endif /* GAME_H */
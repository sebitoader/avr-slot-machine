#ifndef DFPLAYER_H
#define DFPLAYER_H

#include <stdint.h>

/*
 * DFPlayer Mini pe SoftwareSerial:
 *   D7 (PD7) = RX Arduino ← TX DFPlayer
 *   D6 (PD6) = TX Arduino → RX DFPlayer (prin rezistenta 1K)
 */
#define DFP_RX_PIN  7
#define DFP_TX_PIN  6

/* Indexul fisierelor MP3 de pe cardul SD (001.mp3, 002.mp3, ...) */
#define SND_SPIN       1
#define SND_WIN        2
#define SND_LOSE       3
#define SND_JACKPOT    4
#define SND_REDBLACK   5

#define DFP_VOLUME_DEFAULT  25   /* 0–30 */

void dfplayer_init(void);
void dfplayer_play(uint8_t track);
void dfplayer_stop(void);
void dfplayer_set_volume(uint8_t vol);

#endif /* DFPLAYER_H */
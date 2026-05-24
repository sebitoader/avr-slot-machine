#include "dfplayer.h"
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

static SoftwareSerial       dfSerial(DFP_RX_PIN, DFP_TX_PIN);
static DFRobotDFPlayerMini  player;
static uint8_t              df_ok = 0;

void dfplayer_init(void)
{
    dfSerial.begin(9600);
    delay(2000);   /* DFPlayer are nevoie de timp la pornire */

    if (!player.begin(dfSerial)) {
        Serial.println(F("[DFPlayer] FAIL – verifica firele si cardul SD"));
        df_ok = 0;
        return;
    }

    Serial.println(F("[DFPlayer] OK"));
    df_ok = 1;
    player.volume(DFP_VOLUME_DEFAULT);
}

void dfplayer_play(uint8_t track)
{
    if (!df_ok) return;
    player.play(track);
}

void dfplayer_stop(void)
{
    if (!df_ok) return;
    player.stop();
}

void dfplayer_set_volume(uint8_t vol)
{
    if (!df_ok) return;
    if (vol > 30) vol = 30;
    player.volume(vol);
}
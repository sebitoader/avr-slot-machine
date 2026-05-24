#include "game.h"
#include "display.h"
#include "dfplayer.h"
#include "buttons.h"

#include <stdlib.h>
#include <avr/eeprom.h>
#include <Arduino.h>

/* ───────────────────────────────────────────────────────────────
 * Stare globala
 * ─────────────────────────────────────────────────────────────── */
static GameState  state        = STATE_START;
static uint16_t   credits      = STARTING_CREDITS;
static uint8_t    bet          = MIN_BET;
static uint16_t   total_spins  = 0;
static uint16_t   best_win     = 0;
static uint16_t   pending_win  = 0;
static uint8_t    reel[3]      = { SYM_BAR, SYM_BAR, SYM_BAR };

/* Stare pentru hold BLACK (non-blocant) */
static uint8_t    black_hold_active = 0;
static uint32_t   black_held_since  = 0;

/* ───────────────────────────────────────────────────────────────
 * EEPROM
 * ─────────────────────────────────────────────────────────────── */
static void eeprom_save(void)
{
    eeprom_write_word(EEPROM_ADDR_CREDITS, credits);
    eeprom_write_word(EEPROM_ADDR_SPINS,   total_spins);
    eeprom_write_word(EEPROM_ADDR_BEST,    best_win);
    eeprom_write_word(EEPROM_ADDR_MAGIC,   EEPROM_MAGIC_VAL);
}

static void eeprom_load(void)
{
    if (eeprom_read_word(EEPROM_ADDR_MAGIC) != EEPROM_MAGIC_VAL) {
        credits = STARTING_CREDITS; total_spins = 0; best_win = 0;
        eeprom_save();
        Serial.println(F("[EEPROM] Init cu valori default"));
        return;
    }
    credits     = eeprom_read_word(EEPROM_ADDR_CREDITS);
    total_spins = eeprom_read_word(EEPROM_ADDR_SPINS);
    best_win    = eeprom_read_word(EEPROM_ADDR_BEST);
    Serial.print(F("[EEPROM] Credite: ")); Serial.println(credits);
}

/* ───────────────────────────────────────────────────────────────
 * RNG — seed din zgomot ADC pe A0 flotant
 * ─────────────────────────────────────────────────────────────── */
static void rng_seed(void)
{
    uint16_t noise = 0;
    for (uint8_t i = 0; i < 8; i++) {
        noise = (uint16_t)((noise << 1) | (analogRead(A0) & 0x01));
        delay(1);
    }
    srand(noise ^ (uint16_t)micros() ^ credits);
}

/* ───────────────────────────────────────────────────────────────
 * Probabilitati simboluri (din 20):
 *   7=1, BAR=3, STAR=4, CHERRY=6, DIAMOND=6
 * ─────────────────────────────────────────────────────────────── */
static uint8_t rng_symbol(void)
{
    uint8_t r = (uint8_t)(rand() % 20);
    if (r == 0)   return SYM_7;
    if (r <= 3)   return SYM_BAR;
    if (r <= 7)   return SYM_STAR;
    if (r <= 13)  return SYM_CHERRY;
    return               SYM_DIAMOND;
}

/* ───────────────────────────────────────────────────────────────
 * Calcul castig
 *   3x7     → x20    (JACKPOT)
 *   3xBAR   → x10
 *   3xSTAR  → x5
 *   3x alt  → x3
 *   2x orce → x1 (recuperezi miza)
 *   nimic   → 0
 * ─────────────────────────────────────────────────────────────── */
static uint16_t calc_win(uint8_t s1, uint8_t s2, uint8_t s3)
{
    if (s1 == SYM_7 && s2 == SYM_7 && s3 == SYM_7)
        return (uint16_t)bet * 20;
    if (s1 == s2 && s2 == s3) {
        if (s1 == SYM_BAR)  return (uint16_t)bet * 10;
        if (s1 == SYM_STAR) return (uint16_t)bet * 5;
        return (uint16_t)bet * 3;
    }
    if (s1 == s2 || s2 == s3 || s1 == s3) return bet;
    return 0;
}

/* ───────────────────────────────────────────────────────────────
 * Animatie spinning cu decelerare la final
 * ─────────────────────────────────────────────────────────────── */
static void animate_spin(void)
{
    for (uint8_t i = 0; i < SPIN_FRAMES; i++) {
        display_show_spinning();
        uint16_t ms = (i < SPIN_FRAMES - 5)
                    ? SPIN_FRAME_MS
                    : SPIN_FRAME_MS + (uint16_t)(i - (SPIN_FRAMES - 5)) * 35;
        delay(ms);
    }
}

/* ───────────────────────────────────────────────────────────────
 * Eliberare butoane + golire edge flags
 * ─────────────────────────────────────────────────────────────── */
static void wait_release(void)
{
    uint32_t t = millis();
    while (btn_any_held() && (millis() - t) < 600) delay(5);
    delay(25);
    btn_spin_just_pressed();
    btn_bet_just_pressed();
    btn_red_just_pressed();
    btn_black_just_pressed();
    black_hold_active = 0;
}

/* ───────────────────────────────────────────────────────────────
 * Hold BLACK 2s → statistici
 * FIX: verifica direct pinul fizic (nu prin btn_any_held)
 * fara resetare la bounce al altor butoane
 * ─────────────────────────────────────────────────────────────── */
static uint8_t check_black_hold(void)
{
    if (!(PIND & (1 << BTN_BLACK_PIN))) {
        /* BLACK e tinut apasat fizic */
        if (!black_hold_active) {
            black_hold_active = 1;
            black_held_since  = millis();
        } else if ((millis() - black_held_since) >= 2000UL) {
            black_hold_active = 0;
            wait_release();
            display_show_stats(total_spins, best_win);
            state = STATE_STATS;
            Serial.println(F("[State] STATS"));
            return 1;   /* s-a facut tranzitia */
        }
    } else {
        /* BLACK eliberat — reset timer */
        black_hold_active = 0;
    }
    return 0;
}

/* ───────────────────────────────────────────────────────────────
 * game_init
 * ─────────────────────────────────────────────────────────────── */
void game_init(void)
{
    eeprom_load();
    rng_seed();
    bet         = MIN_BET;
    pending_win = 0;
    state       = STATE_START;
    display_show_start();
    Serial.println(F("[Game] Ready"));
}

/* ───────────────────────────────────────────────────────────────
 * game_run — apelat din loop()
 * ─────────────────────────────────────────────────────────────── */
void game_run(void)
{
    buttons_update();

    switch (state) {

    /* ── START ─────────────────────────────────────────────────── */
    case STATE_START:
        if (btn_spin_just_pressed()) {
            wait_release();
            display_clear();
            display_show_reels(reel[0], reel[1], reel[2]);
            display_show_credits(credits, bet);
            state = STATE_IDLE;
            Serial.println(F("[State] IDLE"));
            break;
        }
        check_black_hold();
        break;

    /* ── IDLE ──────────────────────────────────────────────────── */
    case STATE_IDLE:
        if (btn_bet_just_pressed()) {
            bet++;
            if (bet > MAX_BET) bet = MIN_BET;
            display_show_credits(credits, bet);
            Serial.print(F("[Bet] ")); Serial.println(bet);
        }

        if (btn_spin_just_pressed()) {
            wait_release();

            if (credits < bet) {
                display_show_gameover();
                state = STATE_GAMEOVER;
                Serial.println(F("[State] GAMEOVER"));
                break;
            }

            credits    -= bet;
            total_spins++;
            dfplayer_play(SND_SPIN);

            reel[0] = rng_symbol();
            reel[1] = rng_symbol();
            reel[2] = rng_symbol();

            Serial.print(F("[Spin] "));
            Serial.print(reel[0]); Serial.print(' ');
            Serial.print(reel[1]); Serial.print(' ');
            Serial.println(reel[2]);

            animate_spin();
            display_show_reels(reel[0], reel[1], reel[2]);
            dfplayer_stop();
            delay(400);

            uint16_t win = calc_win(reel[0], reel[1], reel[2]);

            if (win > 0) {
                pending_win = win;
                Serial.print(F("[Win] ")); Serial.println(win);

                if (reel[0] == SYM_7 && reel[1] == SYM_7 && reel[2] == SYM_7) {
                    dfplayer_play(SND_JACKPOT);
                    display_show_jackpot();
                    delay(2800);
                    display_show_reels(reel[0], reel[1], reel[2]);
                } else {
                    dfplayer_play(SND_WIN);
                }
                display_show_win(pending_win);
                eeprom_save();
                state = STATE_RESULT;
                Serial.println(F("[State] RESULT"));

            } else {
                dfplayer_play(SND_LOSE);
                display_show_lose();
                display_show_credits(credits, bet);
                eeprom_save();
                delay(1500);
                state = STATE_IDLE;
            }
            break;
        }

        check_black_hold();
        break;

    /* ── RESULT ────────────────────────────────────────────────── */
    case STATE_RESULT:
        /* BET+ = incaseaza */
        if (btn_bet_just_pressed()) {
            credits += pending_win;
            if (pending_win > best_win) best_win = pending_win;
            pending_win = 0;
            eeprom_save();
            display_show_credits(credits, bet);
            state = STATE_IDLE;
            Serial.println(F("[State] IDLE (incasat)"));
        }
        /* SPIN = mergi la Red/Black */
        if (btn_spin_just_pressed()) {
            wait_release();
            dfplayer_play(SND_REDBLACK);
            display_show_reels(reel[0], reel[1], reel[2]);
            display_show_redblack();
            state = STATE_REDBLACK;
            Serial.println(F("[State] REDBLACK"));
        }
        break;

    /* ── RED / BLACK ────────────────────────────────────────────
     * FIX: RED = butonul RED (PD4), BLACK = butonul BLACK (PD5)
     * ─────────────────────────────────────────────────────────── */
    case STATE_REDBLACK: {
        uint8_t chose_red   = btn_red_just_pressed();
        uint8_t chose_black = btn_black_just_pressed();

        if (!chose_red && !chose_black) break;

        wait_release();

        uint8_t result      = (uint8_t)(rand() % 2);   /* 0=Rosu, 1=Negru */
        uint8_t player_wins = (chose_red   && result == 0)
                           || (chose_black && result == 1);

        Serial.print(F("[R/B] Ales="));
        Serial.print(chose_red ? F("Rosu") : F("Negru"));
        Serial.print(F(" Result="));
        Serial.print(result == 0 ? F("Rosu") : F("Negru"));
        Serial.print(F(" Castig="));
        Serial.println(player_wins);

        if (player_wins) {
            pending_win *= 2;
            dfplayer_play(SND_WIN);
            display_show_reels(reel[0], reel[1], reel[2]);
            display_show_win(pending_win);
            state = STATE_RESULT;
        } else {
            pending_win = 0;
            dfplayer_play(SND_LOSE);
            display_show_reels(reel[0], reel[1], reel[2]);
            display_show_lose();
            display_show_credits(credits, bet);
            eeprom_save();
            delay(1500);
            state = STATE_IDLE;
        }
        break;
    }

    /* ── GAMEOVER ──────────────────────────────────────────────── */
    case STATE_GAMEOVER:
        if (btn_spin_just_pressed()) {
            wait_release();
            credits = STARTING_CREDITS; bet = MIN_BET;
            total_spins = 0; best_win = 0;
            eeprom_save();
            rng_seed();
            display_show_start();
            state = STATE_START;
            Serial.println(F("[State] START (reset)"));
        }
        break;

    /* ── STATS ─────────────────────────────────────────────────── */
    case STATE_STATS:
        if (btn_spin_just_pressed()) {
            wait_release();
            display_clear();
            display_show_reels(reel[0], reel[1], reel[2]);
            display_show_credits(credits, bet);
            state = (credits > 0) ? STATE_IDLE : STATE_GAMEOVER;
            Serial.println(F("[State] back from STATS"));
        }
        break;

    default:
        state = STATE_START;
        break;
    }
}
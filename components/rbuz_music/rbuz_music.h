#ifndef __RBUZ_MUSIC_H
#define __RBUZ_MUSIC_H

#include <stdint.h>
#include <string.h>
#include <driver/gpio.h>
#include <driver/rmt.h>
#include <esp_log.h>
#include <soc/rmt_struct.h>
#include <hal/rmt_types.h>

#ifdef __cplusplus
extern "C" {
#endif

// RMT buzzer note structure
typedef struct {
  uint32_t note_hz; /*!< Note frequency, in Hz */
  uint32_t note_ms; /*!< Note duration, in ms */
} rbuz_note_t;

// RMT buzzer music structure
typedef struct {
  rbuz_note_t* notes; /*!< Start pointer of notes in music */
  uint16_t note_num; /*!< Number of notes in music */
  uint16_t next_note_idx; /*!< Index of next note to be played */
  bool repeat; /*!< Restart playing the music after it ends */
  bool stopped; /*!< Music stopped by user */
  #if CONFIG_IDF_TARGET_ESP32
  rmt_item32_t* note_rmti; /*!< Looping note if RMT HW doesn't support it */
  uint32_t note_rmti_num; /*!< RMT loop count RMT HW doesn't support it */
  #endif
} rbuz_music_t;

// RMT buzzer player structure
typedef struct {
  gpio_num_t gpio; /*!< GPIO being used by player */
  rmt_channel_t rmt_channel; /*!< RMT channel being used by player */
  rbuz_music_t music; /*!< Music to be played by player */
  uint32_t rmt_cclk_hz; /*!< Frequency of RMT counter clock being used by player */
} rbuz_player_t;

esp_err_t rbuz_play(rbuz_player_t* player, const rbuz_music_t music);
esp_err_t rbuz_stop(rbuz_player_t* player);
rbuz_music_t rbuz_create_music(const rbuz_note_t* notes, uint16_t note_num, bool repeat);
rbuz_player_t rbuz_create_player(rmt_channel_t rmtc, gpio_num_t buz_gpio);

// Add this as first note if music loops at first note forever when played
#define FNOTE_FIX {20000, 5}, {0, 50}

#define FNOTE_NA  0 // Rest note
#define FNOTE_B0  31
#define FNOTE_C1  33
#define FNOTE_CS1 35
#define FNOTE_D1  37
#define FNOTE_DS1 39
#define FNOTE_E1  41
#define FNOTE_F1  44
#define FNOTE_FS1 46
#define FNOTE_G1  49
#define FNOTE_GS1 52
#define FNOTE_A1  55
#define FNOTE_AS1 58
#define FNOTE_B1  62
#define FNOTE_C2  65
#define FNOTE_CS2 69
#define FNOTE_D2  73
#define FNOTE_DS2 78
#define FNOTE_E2  82
#define FNOTE_F2  87
#define FNOTE_FS2 93
#define FNOTE_G2  98
#define FNOTE_GS2 104
#define FNOTE_A2  110
#define FNOTE_AS2 117
#define FNOTE_B2  123
#define FNOTE_C3  131
#define FNOTE_CS3 139
#define FNOTE_D3  147
#define FNOTE_DS3 156
#define FNOTE_E3  165
#define FNOTE_F3  175
#define FNOTE_FS3 185
#define FNOTE_G3  196
#define FNOTE_GS3 208
#define FNOTE_A3  220
#define FNOTE_AS3 233
#define FNOTE_B3  247
#define FNOTE_C4  262
#define FNOTE_CS4 277
#define FNOTE_D4  294
#define FNOTE_DS4 311
#define FNOTE_E4  330
#define FNOTE_F4  349
#define FNOTE_FS4 370
#define FNOTE_G4  392
#define FNOTE_GS4 415
#define FNOTE_A4  440
#define FNOTE_AS4 466
#define FNOTE_B4  494
#define FNOTE_C5  523
#define FNOTE_CS5 554
#define FNOTE_D5  587
#define FNOTE_DS5 622
#define FNOTE_E5  659
#define FNOTE_F5  698
#define FNOTE_FS5 740
#define FNOTE_G5  784
#define FNOTE_GS5 831
#define FNOTE_A5  880
#define FNOTE_AS5 932
#define FNOTE_B5  988
#define FNOTE_C6  1047
#define FNOTE_CS6 1109
#define FNOTE_D6  1175
#define FNOTE_DS6 1245
#define FNOTE_E6  1319
#define FNOTE_F6  1397
#define FNOTE_FS6 1480
#define FNOTE_G6  1568
#define FNOTE_GS6 1661
#define FNOTE_A6  1760
#define FNOTE_AS6 1865
#define FNOTE_B6  1976
#define FNOTE_C7  2093
#define FNOTE_CS7 2217
#define FNOTE_D7  2349
#define FNOTE_DS7 2489
#define FNOTE_E7  2637
#define FNOTE_F7  2794
#define FNOTE_FS7 2960
#define FNOTE_G7  3136
#define FNOTE_GS7 3322
#define FNOTE_A7  3520
#define FNOTE_AS7 3729
#define FNOTE_B7  3951
#define FNOTE_C8  4186
#define FNOTE_CS8 4435
#define FNOTE_D8  4699
#define FNOTE_DS8 4978

#ifdef __cplusplus
}
#endif

#endif // __RBUZ_MUSIC_H
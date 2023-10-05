#include "rbuz_music.h"

static const char* TAG = "RMT buzzer";

static rmt_item32_t get_next_notation_code(rbuz_player_t* player)
{
  rmt_item32_t notation_code = {.level0 = 1, .duration0 = 1, .level1 = 0, .duration1 = 1};
  rbuz_note_t new_note = player->music.notes[player->music.next_note_idx];

  // Convert note frequency to RMT item format
  if (new_note.note_hz == 0) {
    new_note.note_hz = 10;
    new_note.note_ms *= 2;
    notation_code.level0 = 0;
  }
  notation_code.duration0 = player->rmt_cclk_hz/new_note.note_hz/2;
  notation_code.duration1 = notation_code.duration0;

  uint32_t loop_count = new_note.note_ms*new_note.note_hz/1000;
#if !CONFIG_IDF_TARGET_ESP32
  rmt_set_tx_loop_count(player->rmt_channel, loop_count);
#else
  player->music.note_rmti_num = loop_count;
#endif
  player->music.next_note_idx++;

  return notation_code;
}

static esp_err_t buz_stop(rbuz_player_t* player)
{
  if (player == NULL) return ESP_ERR_INVALID_ARG;

  esp_err_t err = rmt_tx_stop(player->rmt_channel);

  #if CONFIG_IDF_TARGET_ESP32
    if (player->music.note_rmti != NULL) {
      free(player->music.note_rmti);
      player->music.note_rmti = NULL;
    }
  #endif

  return err;
}

static void play_note(rbuz_player_t* player, rmt_item32_t notation_code)
{
#if !CONFIG_IDF_TARGET_ESP32
  rmt_write_items(player->rmt_channel, &notation_code, 1, false);
#else
  player->music.note_rmti = malloc(player->music.note_rmti_num*sizeof(rmt_item32_t));
  for (uint32_t i = 0; i < player->music.note_rmti_num; i++)
  player->music.note_rmti[i] = notation_code;
  rmt_write_items(player->rmt_channel, player->music.note_rmti, player->music.note_rmti_num, false);
#endif
}

// Play music from the beginning
static void play_music_begin(rbuz_player_t* player)
{
  buz_stop(player);
  player->music.stopped = 0;
  player->music.next_note_idx = 0;
  rmt_item32_t notation_code = get_next_notation_code(player);
  play_note(player, notation_code);
}

static void rmt_tx_end_cb(rmt_channel_t channel, void* args)
{
  rbuz_player_t* player = (rbuz_player_t*)args;

  buz_stop(player);

  if (player->music.stopped) return;

  // Update RMT loop freq and duration if the note being played isn't the last
  if (player->music.next_note_idx < player->music.note_num) {
    rmt_item32_t notation_code = get_next_notation_code(player);
    play_note(player, notation_code);
  }
  else if (player->music.next_note_idx == player->music.note_num) {
    if (player->music.repeat) {
      play_music_begin(player);
    }
  }
}

static esp_err_t channel_init(rbuz_player_t* player)
{
  esp_err_t err = ESP_OK;

  // Apply default RMT configuration
  rmt_config_t rmt_cfg = RMT_DEFAULT_CONFIG_TX(player->gpio, player->rmt_channel);
  rmt_cfg.tx_config.loop_en = true; // Enable loop mode

  // Unistall driver if channel is initialized
  rmt_channel_status_result_t result;
  rmt_get_channel_status(&result);
  if (result.status[player->rmt_channel] == RMT_CHANNEL_IDLE ||
      result.status[player->rmt_channel] == RMT_CHANNEL_BUSY)
  err = rmt_driver_uninstall(player->rmt_channel);

  // Install RMT driver
  if (err == ESP_OK) err = rmt_config(&rmt_cfg);
  if (err == ESP_OK) err = rmt_driver_install(player->rmt_channel, 0, 0);
  if (err == ESP_OK) err = rmt_get_counter_clock(player->rmt_channel, &player->rmt_cclk_hz);

  if (err == ESP_OK) {
    rmt_register_tx_end_callback(rmt_tx_end_cb, player);
  }

  if (err) ESP_LOGE(TAG, "Player init failed -> %s", esp_err_to_name(err));

  return err;
}

/**
 * @brief RMT buzzer player start to play the given music
 *
 * @param player Player to use
 * @param music Music to play
 * @return
 *   - ESP_OK: Start playing notation successfully
 *   - ESP_ERR_INVALID_ARG: wrong parameter
 */
esp_err_t rbuz_play(rbuz_player_t* player, const rbuz_music_t music)
{
  if (player == NULL || music.notes == NULL || music.note_num == 0)
  return ESP_ERR_INVALID_ARG;

  player->music = music;
  channel_init(player);
  play_music_begin(player);

  return ESP_OK;
}

/**
 * @brief RMT buzzer player stop
 *
 * @param player Player to stop
 * @return ESP_OK when stop playing is successful
 */
esp_err_t rbuz_stop(rbuz_player_t* player)
{
  esp_err_t err = buz_stop(player);
  if (err == ESP_OK) player->music.stopped = 1;
  return err;
}

/**
 * @brief RMT buzzer player create music to be played
 *
 * @param notes Start pointer of notes to be played
 * @param note_num Number of notes to be played
 * @param repeat Player will repeat playing the music until stopped manually
 * @return Created music structure
 */
rbuz_music_t rbuz_create_music(const rbuz_note_t* notes, uint16_t note_num, bool repeat)
{
  rbuz_music_t music = {0};
  music.notes = (rbuz_note_t*)notes;
  music.note_num = note_num;
  music.repeat = repeat;
  return music;
}

/**
 * @brief RMT buzzer player create
 *
 * @param rmtc RMT channel to be used by player
 * @param buz_gpio Buzzer GPIO to be used by player
 * @return Created player
 */
rbuz_player_t rbuz_create_player(rmt_channel_t rmtc, gpio_num_t buz_gpio)
{
  rbuz_player_t player = {0};
  player.rmt_channel = rmtc;
  player.gpio = buz_gpio;
  return player;
}
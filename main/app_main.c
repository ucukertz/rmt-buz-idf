#include <stdio.h>
#include <sdkconfig.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_spi_flash.h>
#include <esp_log.h>

#include "rbuz_music.h"

static const char* TAG = "RBUZ_MUSIC";
rbuz_player_t player = {0};
rbuz_music_t rmsc_ode_to_joy = {0};

static const rbuz_note_t rnotes_ode_to_joy[] = {
    {740, 400}, {740, 600}, {784, 400}, {880, 400},
    {880, 400}, {784, 400}, {740, 400}, {659, 400},
    {587, 400}, {587, 400}, {659, 400}, {740, 400},
    {740, 400}, {740, 200}, {659, 200}, {659, 800},

    {740, 400}, {740, 600}, {784, 400}, {880, 400},
    {880, 400}, {784, 400}, {740, 400}, {659, 400},
    {587, 400}, {587, 400}, {659, 400}, {740, 400},
    {659, 400}, {659, 200}, {587, 200}, {587, 800},

    {659, 400}, {659, 400}, {740, 400}, {587, 400},
    {659, 400}, {740, 200}, {784, 200}, {740, 400}, {587, 400},
    {659, 400}, {740, 200}, {784, 200}, {740, 400}, {659, 400},
    {587, 400}, {659, 400}, {440, 400}, {440, 400},

    {740, 400}, {740, 600}, {784, 400}, {880, 400},
    {880, 400}, {784, 400}, {740, 400}, {659, 400},
    {587, 400}, {587, 400}, {659, 400}, {740, 400},
    {659, 400}, {659, 200}, {587, 200}, {587, 800},
};

void app_main(void)
{
  player = rbuz_create_player(RMT_CHANNEL_0, GPIO_NUM_4);
  rmsc_ode_to_joy = rbuz_create_music(rnotes_ode_to_joy, sizeof(rnotes_ode_to_joy)/sizeof(rbuz_note_t), 1);
  ESP_LOGI(TAG, "Playing Ode to Joy with repeat");
  rbuz_play(&player, rmsc_ode_to_joy);
}

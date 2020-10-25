/*
 * NeoController.cpp
 *
 *  Created on: 14 Sep 2018
 *      Author: xasin
 */

#include "tef/led/NeoController.h"

namespace TEF {
namespace LED {

rmt_item32_t bits[2] = {};

void init_onoff_bits() {
	bits[0].duration0 = 0.35 * 80 + 2; bits[0].level0 = 1;
	bits[0].duration1 = 1.05 * 80; bits[0].level1 = 0;

	bits[1].duration0 = 0.9  * 80 + 2; bits[1].level0 = 1;
	bits[1].duration1 = 0.5 * 80; bits[1].level1 = 0;
}

static void IRAM_ATTR u8_to_WS2812(const void* source, rmt_item32_t* destination,
	size_t source_size, size_t wanted_elements,
	size_t* translated_size, size_t* translated_items) {

	const int8_t *srcPointer = reinterpret_cast<const int8_t*>(source);

	*translated_size  = 0;
	*translated_items = 0;

	while(*translated_size < source_size && *translated_items < wanted_elements) {
		for(uint8_t i=0; i<8; i++) {
			destination->val = bits[(*srcPointer)>>(7-i) & 1].val;

			(*translated_items)++;
			destination++;
		}

		(*translated_size)++;
		srcPointer++;
	}
}

NeoController::NeoController(gpio_num_t pin, rmt_channel_t channel, uint8_t length) :
		length(length),
		colours(length),
		pinNo(pin), channel(channel) {

	rawColours = new Colour::ColourData[length];

	init_onoff_bits();

	clear();

	gpio_reset_pin(pin);

	rmt_config_t cfg = {};
	rmt_tx_config_t tx_cfg = {};
	tx_cfg.idle_level = RMT_IDLE_LEVEL_LOW;
	tx_cfg.idle_output_en = true;
	tx_cfg.loop_en = false;
	cfg.tx_config = tx_cfg;

	cfg.channel  = channel;
	cfg.rmt_mode = RMT_MODE_TX;
	cfg.clk_div  = 1;
	cfg.gpio_num = pinNo;
	cfg.mem_block_num = 1;

	rmt_config(&cfg);
	rmt_driver_install(channel, 0, 0);
	rmt_translator_init(channel, u8_to_WS2812);

	// gpio_set_drive_capability(pin, GPIO_DRIVE_CAP_3);

	esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, NULL, &powerLock);
}

void NeoController::update() {
	esp_pm_lock_acquire(powerLock);
	for(uint8_t i=0; i<length; i++)
		rawColours[i] = colours[i].getNeopixelData();

	rmt_write_sample(channel, reinterpret_cast<const unsigned char *>(rawColours), length*sizeof(Colour::ColourData), true);

	esp_pm_lock_release(powerLock);
}

void NeoController::fill(Colour colour) {
	for(uint8_t i=0; i<length; i++)
		colours[i] = colour;
}
void NeoController::clear() {
	fill(Colour());
}

Colour& NeoController::operator[](int id) {
	return *get(id);
}
Colour * NeoController::get(int id) {
	return &this->colours[id%length];
}

}
}

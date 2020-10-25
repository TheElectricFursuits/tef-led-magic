/*
 * NeoController.h
 *
 *  Created on: 14 Sep 2018
 *      Author: xasin
 */

#ifndef MAIN_NEOCONTROLLER_H_
#define MAIN_NEOCONTROLLER_H_

#include "driver/rmt.h"
#include "esp_pm.h"
#include "esp32/pm.h"

#include "freertos/task.h"

#include "tef/led/Layer.h"

namespace TEF {
namespace LED {

class NeoController {
public:
	const uint8_t length;

	Layer colours;

private:
	const gpio_num_t pinNo;
	const rmt_channel_t channel;

	Colour::ColourData *rawColours;

	esp_pm_lock_handle_t powerLock;

public:
	NeoController(gpio_num_t pin, rmt_channel_t channel, uint8_t length);

	void update();

	void clear();
	void fill(Colour colour);
	Colour& operator [](int id);
	Colour * get(int id);
};

}
}

#endif /* MAIN_NEOCONTROLLER_H_ */

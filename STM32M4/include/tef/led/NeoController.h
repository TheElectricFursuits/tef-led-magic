/*
 * NeoController.h
 *
 *  Created on: 22 Feb 2020
 *      Author: xasin
 */

#ifndef STM32F4_NEOCONTROLLER_NEOCONTROLLER_H_
#define STM32F4_NEOCONTROLLER_NEOCONTROLLER_H_

#include "Colour.h"
#include "Layer.h"

#include "main.h"

#include <vector>

namespace TEF {
namespace LED {

class NeoController {
	SPI_HandleTypeDef * const spi;

	std::vector<uint8_t> write_buffer;
	uint8_t *current_data;

	void write_u24(uint32_t b);
	void write_u24(const Colour &c, bool swap = false);

public:
	const bool inv_output;
	const int length;

	const uint32_t * rg_swap_map;

	Layer colours;

	NeoController(SPI_HandleTypeDef &spi, int num_leds, bool invert = false);

	void push();
};

}
} /* namespace TEF::LED */

#endif /* STM32F4_NEOCONTROLLER_NEOCONTROLLER_H_ */

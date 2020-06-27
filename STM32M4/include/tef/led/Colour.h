/*
 * Color.h
 *
 *  Created on: 19 Sep 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_TEF_LED_COLOR_H_
#define COMPONENTS_TEF_LED_COLOR_H_

#include <stdint.h>

enum Material : uint32_t {
	BLACK 	= 0x000000,
	RED 	= 0xF42316,
	PINK	= 0xE91E63,
	PURPLE	= 0x9C27B0,
	DEEP_PURPLE = 0x6A3AB7,
	INDIGO	= 0x3F51B5,
	BLUE	= 0x0546FF,
	CYAN	= 0x00CCE4,
	GREEN	= 0x20DF2C,
	LIME	= 0xCCE210,
	YELLOW	= 0xEBEB00,
	AMBER	= 0xFFC007,
	ORANGE	= 0xFF9800,
	DEEP_ORANGE = 0xFF4700
};

namespace TEF {
namespace LED {

class Colour {
private:
	uint8_t f_to_u8(float f) const;

public:
	struct ColourData {
		union {
			struct {
				uint8_t b;
				uint8_t r;
				uint8_t g;
			};
			uint32_t data;
		};
	};

	float r;
	float g;
	float b;
	float alpha;

	static Colour HSV(float H, float S = 1, float V = 1);

	Colour();
	Colour(uint32_t cCode, float brightness = 1);
	Colour(uint32_t cCode, float brightness, float alpha);

	ColourData getNeopixelData() const;
	uint32_t   getPrintable() const;

	template<int depth>
	uint32_t getHUB75Data() const {
		uint32_t out = 0;

		auto c_data = getNeopixelData();
		uint8_t local_r = c_data.r;
		uint8_t local_g = c_data.g;
		uint8_t local_b = c_data.b;

		for(uint8_t mask = 0x80 >> (depth-1); mask != 0; mask <<= 1) {
			out <<= 3;

			if(mask & local_r)
				out |= 0b001;
			if(mask & local_g)
				out |= 0b010;
			if(mask & local_b)
				out |= 0b100;
		}

		return out;
	};

	void set(uint32_t cCode);
	void set(uint32_t cCode, float bMod);
	void set(const Colour &color);

	Colour& operator=(const Colour& nColor);

	Colour& bMod(float div);
	Colour  bMod(float div) const;

	Colour overlay(Colour topColor, float alpha);
	void  overlay(Colour bottom, Colour top, float alpha);

	Colour operator +(Colour secondColor);
	Colour operator *(float brightness);

	Colour& merge_overlay(const Colour &top, float alpha = 1);
	Colour& merge_multiply(const Colour &top, float alpha = 1);
	Colour& merge_multiply(float scalar);
	Colour& merge_add(const Colour &top, float alpha = 1);

	Colour& merge_transition(const Colour &top, float alpha);

	Colour calculate_overlay(const Colour &top, float alpha = 1) const;
	Colour calculate_multiply(const Colour &top, float alpha = 1) const;
	Colour calculate_multiply(float scalar) const;
	Colour calculate_add(const Colour &top, float alpha = 1) const;
};

}
} /* namespace Peripheral */

#endif /* COMPONENTS_NEOCONTROLLER_COLOR_H_ */

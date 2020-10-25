/*
 * MatrixSliceBuffer.h
 *
 *  Created on: 22 Jun 2020
 *      Author: xasin
 */

#ifndef TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIXSLICEBUFFER_H_
#define TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIXSLICEBUFFER_H_

#include <array>

namespace TEF {
namespace LED {
namespace Matrix {

template<int height, int width>
class SliceBuffer {
public:
	std::array<uint8_t, height/2 * width> led_data;

	SliceBuffer() : led_data() {
	}

	constexpr int row_count() {
		return height / 2;
	}
	constexpr int row_length() {
		return width;
	}

	uint32_t row_start_address(int row) {
		if(row >= row_count())
			return 0;

		return reinterpret_cast<uint32_t>(led_data.data() + width * row);
	}

	void set_colour(unsigned int x, unsigned int y, uint8_t bits) {
		if(x >= width)
			return;
		if(y >= height)
			return;

		uint8_t b_shift = (y >> (height == 16 ? 3 : 4)) * 3;
		uint8_t row = y & ((height == 16 ? 0b111 : 0b1111));

		led_data[row * width + x] &= ~(0b111 << b_shift);
		led_data[row * width + x] |= (bits & 0b111) << b_shift;
	}

	void clear() {
		led_data.fill(0);
	}
};


}
}
}

#endif /* TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIXSLICEBUFFER_H_ */

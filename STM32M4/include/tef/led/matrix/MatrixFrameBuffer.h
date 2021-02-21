/*
 * MatrixFrameBuffer.h
 *
 *  Created on: 22 Jun 2020
 *      Author: xasin
 */

#ifndef TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIX_MATRIXFRAMEBUFFER_H_
#define TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIX_MATRIXFRAMEBUFFER_H_

#include "MatrixSliceBuffer.h"
#include "tef/led/GenericMatrix.h"

namespace TEF {
namespace LED {
namespace Matrix {

template<int t_height, int t_width, int depth>
class FrameBuffer {
public:
	std::array<SliceBuffer<t_height, t_width>, depth> slices;

	FrameBuffer() : slices() {}

	static constexpr int row_count() {
		return t_height / 2;
	}

	static constexpr int virtual_row_count() {
		return row_count() * ((1 << depth) - 1);
	}

//	constexpr uint32_t row_start_address(int row) {
//		int slice_row = (row / ((1<<depth)-1)) & (row_count() - 1);
//
//		// Calculate the current slice to use based on
//		// binary pulse modulation.
//		// For 4-bit depth this means:
//		//  - 1*** -> Slice 3
//		//  - 01** -> Slice 2
//		//  - 001* -> Slice 1
//		//  - 0001 -> Slice 0
//		// Where the binary number here is (row) % (1<<depth) + 1
//		//
//		// By giving each row the same time, the slices are varied according to
//		// binary pulse modulation.
//
//		uint32_t slice_bits  = row - slice_row * ((1<<depth)-1) + 1;
//
//		int i = depth-1;
//		for(uint32_t mask = 1 << (depth-1); mask != 0; mask >>= 1) {
//			if(mask & slice_bits)
//				return slices[i].row_start_address(slice_row);
//
//			i--;
//		}
//
//		return 0;
//	}

	uint32_t row_start_address(int physical_row, uint8_t bit) {
		return slices[bit].row_start_address(physical_row);
	}

	void set_colour(int x, int y, const Colour &c) {
		if(x >= t_width || x < 0)
			return;
		if(y >= t_height || y < 0)
			return;

		uint8_t row = y & ((t_height == 16 ? 0b111 : 0b1111));

		int write_offset = row * t_width + x;

		uint8_t mask = 0b111;

		uint32_t c_data = c.getHUB75Data<depth>();

		if(y >> (t_height == 16 ? 3 : 4)) {
			c_data    <<= 3;
			mask <<= 3;
		}

		for(int i=depth-1; i>=0; i--) {
			slices[i].led_data[write_offset] &= ~mask;
			slices[i].led_data[write_offset] |= c_data & mask;

			c_data >>= 3;
		}
	}

	void clear() {
		for(auto &b : slices)
			b.clear();
	}
};


}
}
}


#endif /* TEF_LED_MAGIC_INCLUDE_TEF_LED_MATRIX_MATRIXFRAMEBUFFER_H_ */

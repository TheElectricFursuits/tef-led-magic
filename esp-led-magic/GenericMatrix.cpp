/*
 * GenericMatrix.cpp
 *
 *  Created on: 22 Jun 2020
 *      Author: xasin
 */

#include <tef/led/GenericMatrix.h>

#include <cstring>
#include <algorithm>

namespace TEF {
namespace LED {

GenericMatrix::GenericMatrix(int width, int height) : width(width), height(height) {
}

void GenericMatrix::clear() {}

void GenericMatrix::set_colour(int x, int y, const ColourGradient &c) { }

void GenericMatrix::draw_bitmap(const uint8_t *start, int x, int y, int width, int height, const ColourGradient &c) {
	for(int row = y; row < y+height; row++) {
		uint8_t mask = 1<<7;
		uint8_t current_row = *(start++);

		for(int column = x; column < x+width; column++) {
			if(mask == 0) {
				mask = 1<<7;
				current_row = *(start++);
			}

			if(mask & current_row)
				set_colour(column, row, c);

			mask >>= 1;
		}
	}
}

void GenericMatrix::draw_string(const char *str, const uint8_t *charmap, int x, int y, int char_width, int char_height, const ColourGradient &c) {
	int str_len = strlen(str);

	int leftmost_char  = std::max(0, (char_width - x) / char_width - 1);
	int rightmost_char = std::min(str_len-1, (this->width + char_width - 1 - x) / char_width);

	int char_b_num = (char_width+7) / 8;

	for(int i = leftmost_char; i <= rightmost_char; i++) {
		int pos = x + char_width * i;
		draw_bitmap(charmap + str[i] * char_b_num*char_height, pos, y, char_width, char_height, c);
	}
}

}
}

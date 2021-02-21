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

void GenericMatrix::set_colour(int x, int y, const Colour &c) {}

void GenericMatrix::overlay_colour(int x, int y, const Colour &c) {}

void GenericMatrix::draw_bitmap(const uint8_t *start, int x, int y, int width, int height, const Colour &c) {
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

void GenericMatrix::draw_string(const char *str, const uint8_t *charmap, int x, int y, int char_width, int char_height, const Colour &c) {
	int str_len = strlen(str);

	int leftmost_char  = std::max(0, (char_width - x) / char_width - 1);
	int rightmost_char = std::min(str_len-1, (this->width + char_width - 1 - x) / char_width);

	int char_b_num = (char_width+7) / 8;

	for(int i = leftmost_char; i <= rightmost_char; i++) {
		int pos = x + char_width * i;
		draw_bitmap(charmap + str[i] * char_b_num*char_height, pos, y, char_width, char_height, c);
	}
}

void GenericMatrix::draw_dot(const point_t &point, const Colour &draw_colour, float radius) {
	for(int y_pos = floorf(point.y - radius); y_pos <= ceilf(point.y + radius); y_pos++) {
		for(int x_pos = floorf(point.x - radius); x_pos <= ceilf(point.x + radius); x_pos++) {
			point_t draw_pos = {x_pos, y_pos};
			float dist = (draw_pos - point).length() / radius;
			if(dist > 1)
				continue;

			Colour c = draw_colour;
			c.alpha *= 1 - dist;

			overlay_colour(draw_pos.x, draw_pos.y, c);
		}
	}
}

void GenericMatrix::draw_line(const line_t &line, const Decorator &decoration) {
	const line_dist_calc_t dist_calc(line);
	const point_t &norm = dist_calc.normal_direction;

	bool horizontal = fabsf(norm.x) > fabsf(norm.y);

	int scan_start = floorf(horizontal ? line.min_x() : line.min_y());
	int scan_end   = ceilf(horizontal ?  line.max_x() : line.max_y());

	float scan_orth_inclination = horizontal ? (norm.y / norm.x) : (norm.x / norm.y);
	float scan_orth_offset      = horizontal ? (line.start.y - line.start.x * scan_orth_inclination)
			: (line.start.x - line.start.y * scan_orth_inclination);

	for(int scan_pos = scan_start; scan_pos <= scan_end; scan_pos++) {

		int scan_orth_start = floorf(scan_pos * scan_orth_inclination + scan_orth_offset);
		for(float scan_orth_pos = scan_orth_start; scan_orth_pos <= scan_orth_start+1; scan_orth_pos++) {
			point_t draw_pos = horizontal ? point_t({scan_pos, scan_orth_pos})
					: point_t({scan_orth_pos, scan_pos});

			float line_dist 	= dist_calc.dist_from(draw_pos) * 0.97;
			float line_progress = dist_calc.dist_along(draw_pos);

			LED::Colour draw_colour = decoration.get_at(draw_pos, line_progress);

			if(fabsf(line_dist) < 1)
				draw_colour.alpha *= 1-fabsf(line_dist);
			else
				continue;

			if(line_progress < -1)
				continue;
			else if(line_progress < 0)
				draw_colour.alpha *= 1 + line_progress;

			float line_excess = line_progress - dist_calc.length;
			if(line_excess > 1)
				continue;
			else if(line_excess > 0)
				draw_colour.alpha *= 1 - line_excess;


			overlay_colour(draw_pos.x, draw_pos.y, draw_colour);
		}
	}
}

void GenericMatrix::draw_polygon(const polygon_t &poly, const Decorator &decoration) {
	ScaledDecorator offset_decorator(decoration, 1 / poly.get_length(), 0);

	for(auto l : poly.get_lines()) {
		draw_line(l, offset_decorator);

		offset_decorator.offset += l.length() / poly.get_length();
	}
}

void row_insert_intersection(float * row_intsecs, int row_num, float x_intsec) {
	float * row_ptr = row_intsecs + 17 * row_num;

	for(int i=1; i <= *row_ptr; i++) {
		if(row_ptr[i] > x_intsec) {
			float temp = row_ptr[i];
			row_ptr[i] = x_intsec;
			x_intsec = temp;
		}
	}

	*row_ptr += 1;
	row_ptr[int(roundf(*row_ptr))] = x_intsec;
}

void GenericMatrix::fill_polygon(const polygon_t &poly, const Decorator &decoration) {
	const int row_end = floorf(poly.get_bottom_right().y);
	const int row_start = floorf(poly.get_top_left().y);

	const int num_affected_rows = row_end - row_start + 1;

	float * row_intersections = new float[num_affected_rows * 17];
	for(int i=0; i<num_affected_rows*17; i++)
		row_intersections[i] = 0;

	for(auto l : poly.get_lines()) {
		if(l.end.y == l.start.y)
			continue;

		float x_intsec_slope  = (l.end.x - l.start.x) / (l.end.y - l.start.y);
		float x_intsec_offset = l.start.x - x_intsec_slope * l.start.y;

		for(int y_pos = ceilf(l.min_y()); y_pos <= floorf(l.max_y()); y_pos++) {
			float x_intersection = y_pos * x_intsec_slope + x_intsec_offset;
			row_insert_intersection(row_intersections, y_pos - row_start, x_intersection);
		}
	}

	for(int y_pos = row_start; y_pos <= row_end; y_pos++) {
		float * intersections = row_intersections + 17*(y_pos - row_start);

		int total_pairs = roundf(*intersections) / 2;

		for(int i=0; i<total_pairs; i++) {
			for(int x_pos = ceilf(intersections[1 + i*2]); x_pos <= floorf(intersections[2 + i*2]); x_pos++)
				overlay_colour(x_pos, y_pos, decoration.get_at(point_t({x_pos, y_pos}), 0));
		}
	}

	delete row_intersections;
}

}
}

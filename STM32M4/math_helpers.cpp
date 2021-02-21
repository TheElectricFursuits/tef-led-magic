/*
 * math_helpers.cpp
 *
 *  Created on: 16 Feb 2021
 *      Author: xasin
 */


#include <tef/led/math_helpers.h>
#include <cstring>

namespace TEF {
namespace LED {

float ease_func(float progress) {
	if(progress < 0)
		return 0;
	if(progress > 1)
		return 1;

	return progress < 0.5F ? 4.0F * powf(progress, 3) : 1.0F - powf(-2.0F * progress + 2.0F, 3.0F) / 2.0F;
}

point_t point_t::rotate(float angle, const point_t &centerpoint) const {
	return rotate(cosf(angle), sinf(angle), centerpoint);
}

float line_t::dist_along(const point_t &point) const {
	const point_t dir_vect = (end - start).norm();

	return (point - start) * dir_vect - start * dir_vect;
}

float line_t::dist_from(const point_t &point) const {
	const point_t dir_vect = (end - start).norm();

	return (point - start).cross_p(dir_vect) - start.cross_p(dir_vect);
}

float line_t::length() const {
	return (end - start).length();
}

point_t line_t::norm() const {
	return (end - start).norm();
}

line_dist_calc_t::line_dist_calc_t(const line_t &line) :
	length(line.length()),
	normal_direction((line.end - line.start) / length),
	cross_offset(line.start.cross_p(normal_direction)),
	dot_offset(line.start * normal_direction) {
}

float line_dist_calc_t::dist_along(const point_t &point) const {
	return point * normal_direction - dot_offset;
}
float line_dist_calc_t::dist_from(const point_t &point) const {
	return point.cross_p(normal_direction) - cross_offset;
}

polygon_t::polygon_t() : lines(), length(0), top_left(), bottom_right() {}

polygon_t::polygon_t(std::initializer_list<line_t> line_init) :
		lines(line_init) {

	recalculate_bounds();
	recalculate_length();
}

polygon_t polygon_t::from_points(std::initializer_list<point_t> point_init) {
	polygon_t out_poly;

	out_poly.lines.reserve(point_init.size() / 2);
	point_t start_point = *(point_init.end() - 1);
	for(const point_t &p : point_init) {
		out_poly.lines.push_back({start_point, p});
		start_point = p;
	}

	out_poly.recalculate_bounds();
	out_poly.recalculate_length();

	return out_poly;
}

polygon_t polygon_t::from_svg(const char *str) {
	polygon_t out_poly;

	point_t start_pos = {0, 0};
	point_t position = {0, 0};

	const char *str_end = strchr(str, '\0');

	while(str != nullptr && str <= str_end) {
		if(*str == 'M') {
			char *char_ptr;
			str++;

			position.x = strtof(str, &char_ptr);
			position.y = strtof(char_ptr, &char_ptr);

			start_pos = position;

			str = char_ptr;
		}
		else if(*str == 'L') {
			point_t next_pos;

			str++;
			char *char_ptr;

			next_pos.x = strtof(str, &char_ptr);
			next_pos.y = strtof(char_ptr, &char_ptr);

			str = char_ptr;

			out_poly.lines.push_back({position, next_pos});

			position = next_pos;
		}
		else if(*str == 'Z') {
			out_poly.lines.push_back({position, start_pos});
			str++;
		}
		else
			str = strpbrk(str, "MLZ");
	}

	out_poly.recalculate_bounds();
	out_poly.recalculate_length();

	return out_poly;
}

void polygon_t::recalculate_bounds() {
	bottom_right = lines[0].start;
	top_left = lines[0].end;

	for(auto p : lines) {
		bottom_right.x = std::max(bottom_right.x, p.max_x());
		bottom_right.y = std::max(bottom_right.y, p.max_y());

		top_left.x = std::min(top_left.x, p.min_x());
		top_left.y = std::min(top_left.y, p.min_y());
	}
}

void polygon_t::recalculate_length() {
	length = 0;

	for(auto l : lines) {
		length += l.length();
	}
}

polygon_t polygon_t::operator +(const point_t &point) const& {
	polygon_t polycopy = *this;
	polycopy += point;

	return polycopy;
}
polygon_t &polygon_t::operator +=(const point_t &point) {
	for(auto &p : lines)
		p += point;

	top_left += point;
	bottom_right += point;

	return *this;
}

polygon_t polygon_t::operator -(const point_t &point) const& {
	polygon_t polycopy = *this;
	polycopy -= point;

	return polycopy;
}
polygon_t &polygon_t::operator -=(const point_t &point) {
	for(auto &p : lines)
		p -= point;

	top_left -= point;
	bottom_right -= point;

	return *this;
}

polygon_t polygon_t::operator *(float scalar) const& {
	polygon_t polycopy = *this;
	polycopy *= scalar;

	return polycopy;
}
polygon_t &polygon_t::operator *=(float scalar) {
	for(auto &p : lines)
		p *= scalar;

	top_left *= scalar;
	bottom_right *= scalar;

	length *= scalar;

	return *this;
}

polygon_t &polygon_t::merge_inplace(const polygon_t &source, float amount) {
	if(amount <= 0)
		return *this;

	if(amount >= 1) {
		(*this) = source;
		return *this;
	}

	if(source.lines.size() != lines.size())
		return *this;

	for(unsigned int i=0; i<lines.size(); i++) {
		auto &l = lines[i];
		auto &l_s = source.lines[i];

		l *= (1-amount);
		l += l_s * amount;
	}

	recalculate_bounds();
	recalculate_length();

	return *this;
}

polygon_t polygon_t::scale_copy(float scalar, const point_t &point) const& {
	polygon_t polycopy = *this;

	polycopy.scale_inplace(scalar, point);
	return polycopy;
}
polygon_t &polygon_t::scale_inplace(float scalar, const point_t &centerpoint) {
	*this -= centerpoint;
	*this *= scalar;
	*this += centerpoint;

	return (*this);
}

polygon_t polygon_t::rotate_copy(float angle, const point_t &centerpoint) const& {
	polygon_t polycopy = *this;
	polycopy.rotate_inplace(angle, centerpoint);

	return polycopy;
}

polygon_t &polygon_t::rotate_inplace(float angle, const point_t &centerpoint) {
	const float cos_fact = cosf(angle);
	const float sin_fact = sinf(angle);

	for(auto &l : lines)
		l = l.rotate(cos_fact, sin_fact, centerpoint);

	recalculate_bounds();

	return *this;
}

point_t polygon_t::get_point_at(float length) {
	length = fmodf(length, this->length);

	for(auto l : lines) {
		const float line_len = l.length();
		if(length < line_len)
			return (l.start * (1 - length / line_len)) + l.end * (length / line_len);

		length -= line_len;
	}

	return {0, 0};
}

}
}

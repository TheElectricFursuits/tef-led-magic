/*
 * Decorator.cpp
 *
 *  Created on: 18 Feb 2021
 *      Author: xasin
 */

#include <tef/led/Decorator.h>

#include <math.h>

namespace TEF {
namespace LED {

Decorator::Decorator() : base_colour(0, 0, 0) {
}
Decorator::Decorator(const Colour &base_colour) : base_colour(base_colour) {
}

Decorator::~Decorator() {
}

Colour Decorator::get_at(float lin) const {
	return get_at({lin, lin}, lin);
}
Colour Decorator::get_at(const point_t &pos, float lin) const {
	return base_colour;
}

ScaledDecorator::ScaledDecorator(const Decorator &top, float scaling, float offset) :
		top_decorator(top), scaling(scaling), offset(offset) {
}

Colour ScaledDecorator::get_at(const point_t &pos, float lin) const {
	return top_decorator.get_at(pos, lin * scaling + offset);
}

DropDecorator::DropDecorator(const Decorator &base, const Colour &drop_colour, float drop_len, float tail_len)
	: base_decorator(base), drop_length(drop_len), tail_length(tail_len), offset(0),
	  drop_colour(drop_colour), tail_colour(drop_colour) {
}

Colour DropDecorator::get_at(const point_t &pos, float lin) const {
	float offset_lin = fmodf((lin - offset), 1);
	if(offset_lin > 0)
		offset_lin -= 1;

	if(offset_lin > -drop_length)
		return drop_colour;
	else if(offset_lin > -tail_length) {
		Colour base_colour = base_decorator.get_at(lin);

		base_colour.merge_transition(tail_colour, 1 + offset_lin/tail_length);

		return base_colour;
	}
	else
		return base_decorator.get_at(lin);
}

RainbowDecorator::RainbowDecorator() : Decorator(Material::RED), scaling(1), offset(0) {}

Colour RainbowDecorator::get_at(const point_t &pos, float lin) const {
	return Colour::HSV(pos.y * scaling + offset);
}

} /* namespace LED */
} /* namespace TEF */

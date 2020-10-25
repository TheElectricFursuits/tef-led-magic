/*
 * ColourGradient.cpp
 *
 *  Created on: 22 Jun 2020
 *      Author: xasin
 */

#include <tef/led/ColourGradient.h>


namespace TEF {
namespace LED {

ColourGradient::ColourGradient() : default_colour(nullptr) {
}
ColourGradient::ColourGradient(const Colour &c) : default_colour(&c) {

}
Colour ColourGradient::get_colour_at(float pos) const {
	if(default_colour)
		return *default_colour;

	return Colour(0, 0, 0);
}

RainbowGradient::RainbowGradient() : ColourGradient() {}
Colour RainbowGradient::get_colour_at(float pos) const {
	return Colour::HSV(pos * 10);
}

}
}


/*
 * ColourGradient.h
 *
 *  Created on: 22 Jun 2020
 *      Author: xasin
 */

#ifndef TEF_LED_MAGIC_INCLUDE_TEF_LED_COLOURGRADIENT_H_
#define TEF_LED_MAGIC_INCLUDE_TEF_LED_COLOURGRADIENT_H_

#include "Colour.h"

namespace TEF {
namespace LED {

class ColourGradient {
private:
	const Colour * const default_colour;

public:
	ColourGradient();
	ColourGradient(const Colour &c);

	virtual Colour get_colour_at(float pos) const;
};

class RainbowGradient : public ColourGradient {
public:
	RainbowGradient();

	Colour get_colour_at(float pos) const;
};

}
}


#endif /* TEF_LED_MAGIC_INCLUDE_TEF_LED_COLOURGRADIENT_H_ */

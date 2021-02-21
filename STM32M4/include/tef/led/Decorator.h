/*
 * Decorator.h
 *
 *  Created on: 18 Feb 2021
 *      Author: xasin
 */

#ifndef TEF_LED_MAGIC_DECORATOR_H_
#define TEF_LED_MAGIC_DECORATOR_H_

#include "Colour.h"
#include "math_helpers.h"

namespace TEF {
namespace LED {

class Decorator {
protected:
	Colour base_colour;

public:
	Decorator();
	Decorator(const Colour &base_colour);

	virtual ~Decorator();

	virtual Colour get_at(float lin) const;
	virtual Colour get_at(const point_t &pos, float lin = nan("")) const;
};

class ScaledDecorator : public Decorator {
private:
	Decorator const &top_decorator;

public:
	float scaling;
	float offset;

	ScaledDecorator(const Decorator &top, float scaling, float offset);

	Colour get_at(const point_t &pos, float lin) const;
};

class DropDecorator : public Decorator {
private:
	Decorator const &base_decorator;

public:
	float drop_length;
	float tail_length;

	float offset;

	Colour drop_colour;
	Colour tail_colour;

	DropDecorator(const Decorator &base, const Colour &drop_colour, float drop_length = 1, float tail_length = 3);

	Colour get_at(const point_t &pos, float lin) const;
};

class RainbowDecorator : public Decorator {
public:
	float scaling;
	float offset;

	RainbowDecorator();

	Colour get_at(const point_t &pos, float lin) const;
};

} /* namespace LED */
} /* namespace TEF */

#endif /* TEF_LED_MAGIC_DECORATOR_H_ */

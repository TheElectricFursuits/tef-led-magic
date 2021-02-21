/*
 * GenericGraphicsPane.h
 *
 *  Created on: 16 Feb 2021
 *      Author: xasin
 */

#ifndef TEF_LED_MAGIC_INCLUDE_TEF_LED_GENERICGRAPHICSPANE_H_
#define TEF_LED_MAGIC_INCLUDE_TEF_LED_GENERICGRAPHICSPANE_H_

#include "math_helpers.h"
#include "Decorator.h"

namespace TEF {
namespace LED {

class GenericGraphicsPane {
public:
	GenericGraphicsPane();
	virtual ~GenericGraphicsPane();

	virtual void draw_dot(const point_t &point, const Colour &c, float radius);

	virtual void draw_line(const line_t &line, const Decorator &decoration);
	virtual void draw_polygon(const polygon_t &polygon, const Decorator &decoration);

	virtual void fill_polygon(const polygon_t &polygon, const Decorator &decoration);
};

} /* namespace LED */
} /* namespace TEF */

#endif /* TEF_LED_MAGIC_INCLUDE_TEF_LED_GENERICGRAPHICSPANE_H_ */

/*
 * GenericMatrix.h
 *
 *  Created on: 22 Jun 2020
 *      Author: xasin
 */

#ifndef TEF_LED_MAGIC_INCLUDE_TEF_LED_GENERICMATRIX_H_
#define TEF_LED_MAGIC_INCLUDE_TEF_LED_GENERICMATRIX_H_

#include <stdint.h>
#include "GenericGraphicsPane.h"

#include "Decorator.h"

namespace TEF {
namespace LED {

class GenericMatrix : public GenericGraphicsPane {
public:
	const int width;
	const int height;

	GenericMatrix(int width, int height);

	virtual void set_colour(int x, int y, const Colour &c);

	virtual void overlay_colour(int x, int y, const Colour &c);

	void draw_bitmap(const uint8_t * start, int x, int y, int width, int height, const Colour &c);
	void draw_string(const char *str, const uint8_t *charmap, int x, int y, int width, int height, const Colour &c);

	void draw_dot(const point_t &point, const Colour &c, float radius);

	void draw_line(const line_t &line, const Decorator &decoration);
	void draw_polygon(const polygon_t &poly, const Decorator &decorator);

	void fill_polygon(const polygon_t &poly, const Decorator &decoration = Decorator(LED::Colour(Material::RED)));

	virtual void clear();
};

}
}



#endif /* TEF_LED_MAGIC_INCLUDE_TEF_LED_GENERICMATRIX_H_ */

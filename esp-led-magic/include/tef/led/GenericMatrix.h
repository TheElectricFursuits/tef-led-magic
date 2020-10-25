/*
 * GenericMatrix.h
 *
 *  Created on: 22 Jun 2020
 *      Author: xasin
 */

#ifndef TEF_LED_MAGIC_INCLUDE_TEF_LED_GENERICMATRIX_H_
#define TEF_LED_MAGIC_INCLUDE_TEF_LED_GENERICMATRIX_H_

#include <stdint.h>

#include "ColourGradient.h"

namespace TEF {
namespace LED {

class GenericMatrix {
public:
	const int width;
	const int height;

	GenericMatrix(int width, int height);

	virtual void set_colour(int x, int y, const ColourGradient &c);

	void draw_bitmap(const uint8_t * start, int x, int y, int width, int height, const ColourGradient &c);
	void draw_string(const char *str, const uint8_t *charmap, int x, int y, int width, int height, const ColourGradient &c);

	virtual void clear();
};

}
}



#endif /* TEF_LED_MAGIC_INCLUDE_TEF_LED_GENERICMATRIX_H_ */

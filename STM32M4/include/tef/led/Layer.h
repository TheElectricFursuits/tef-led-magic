/*
 * Colour.h
 *
 *  Created on: 19 Sep 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_TEF_LED_LAYER_H_
#define COMPONENTS_TEF_LED_LAYER_H_

#include <tef/led/Colour.h>
#include <vector>


namespace TEF {
namespace LED {

class Layer {
	public:
		std::vector<Colour> colours;

		float alpha;

		Layer(const int length);
		Layer(const Layer &source);
		Layer(Colour Colour);

		int length() const;

		Colour& get(int id);
		Colour& operator[](int id);
		Colour  operator[](int id) const;
		Layer& operator=(const Layer& source);

		Layer& fill(Colour fColour, int from = 0, int to = -1);

		Layer& merge_overlay(const Layer &top, int offset = 0, bool wrap = false);
		Layer& merge_multiply(const Layer &top, int offset = 0, bool wrap = false);
		Layer& merge_multiply(const std::vector<uint8_t> &scalars, int offset=0, bool wrap = false);
		Layer& merge_add(const Layer &top, int offset = 0, bool wrap = false);
		Layer& merge_transition(const Layer &top, int offset = 0, bool wrap = false);

		Layer& alpha_set(const std::vector<float> &newAlphas);

//		Layer& calculate_overlay(const Layer &top, int offset = 0, bool wrap = false) const;
//		Layer& calculate_multiply(const Layer &top, int offset = 0, bool wrap = false) const;
//		Layer& calculate_multiply(const uint8_t *scalars) const;
//		Layer& calculate_add(const Layer &top, int offset = 0, bool wrap = false) const;

};
}
}

#endif

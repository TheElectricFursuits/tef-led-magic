/*
 * GenericGraphicsPane.cpp
 *
 *  Created on: 16 Feb 2021
 *      Author: xasin
 */

#include <tef/led/GenericGraphicsPane.h>

namespace TEF {
namespace LED {

GenericGraphicsPane::GenericGraphicsPane() {
}

GenericGraphicsPane::~GenericGraphicsPane() {
}

void GenericGraphicsPane::draw_dot(const point_t &point, const Colour &c, float radius) {}

void GenericGraphicsPane::draw_line(const line_t &line, const Decorator &decoration) {}
void GenericGraphicsPane::draw_polygon(const polygon_t &polygon, const Decorator &decoration) {}

void GenericGraphicsPane::fill_polygon(const polygon_t &polygon, const Decorator &decoration) {}

} /* namespace LED */
} /* namespace TEF */

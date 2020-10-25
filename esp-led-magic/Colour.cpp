/*
 * Colour.cpp
 *
 *  Created on: 19 Sep 2018
 *      Author: xasin
 */


#include <tef/led/Colour.h>
#include <cmath>

namespace TEF {
namespace LED {

Colour Colour::HSV(float H, float S, float V) {
	H = fmodf(H, 360);
	if(H < 0)
		H += 360;

	uint8_t h = floorf(H)/60;

	float f = (H - 60*h)/60.0f;

	float p = V * (1 - S);

	float q = V * (1 - S*f);
	float t = V * (1 - S*(1 - f));

	Colour oC = Colour();

	switch(h) {
	default:oC.r = V; oC.g = t; oC.b = p; break;
	case 1: oC.r = q; oC.g = V; oC.b = p; break;
	case 2: oC.r = p; oC.g = V; oC.b = t; break;
	case 3: oC.r = p; oC.g = q; oC.b = V; break;
	case 4: oC.r = t; oC.g = p; oC.b = V; break;
	case 5: oC.r = V; oC.g = p; oC.b = q; break;
	}

	return oC;
}

Colour::Colour() {
	r = 0;
	g = 0;
	b = 0;

	alpha = 1;
}
Colour::Colour(uint32_t cCode, float brightness) : Colour() {
	set(cCode);
	bMod(brightness);
}
Colour::Colour(uint32_t cCode, float brightness, float alpha) : Colour(cCode, brightness) {
	this->alpha = alpha;
}

uint8_t Colour::f_to_u8(float f) const {
	if(f > 1.0f)
		return 255;
	if(f < 0.0f)
		return 0;

	return roundf(f*f*(255));
}

Colour::ColourData Colour::getNeopixelData() const {
	ColourData out = {};

	out.r = f_to_u8(r);
	out.g = f_to_u8(g);
	out.b = f_to_u8(b);

	return out;
}

uint32_t Colour::getPrintable() const {
	auto data = this->getNeopixelData();

	return (data.r*255) << 16 | (data.g*255) << 8 | (data.b*255);
}

void Colour::set(const Colour &Colour) {
	r = Colour.r;
	g = Colour.g;
	b = Colour.b;

	alpha = Colour.alpha;
}
void Colour::set(uint32_t cCode) {
	uint8_t *ColourPart = (uint8_t *)&cCode;

	alpha = 1 - ColourPart[3]/255.0f;

	r = ColourPart[2]/255.0f;
	g = ColourPart[1]/255.0f;
	b = ColourPart[0]/255.0f;
}
void Colour::set(uint32_t cCode, float factor) {
	set(cCode);
	bMod(factor);
}
Colour& Colour::operator=(const Colour& nColour) {
	this->set(nColour);

	return *this;
}

Colour &Colour::bMod(float factor) {
	r = r*factor;
	g = g*factor;
	b = b*factor;

	return *this;
}
Colour Colour::bMod(float factor) const {
	Colour oColour = *this;

	return oColour.bMod(factor);
}

Colour Colour::overlay(Colour topColour, float alpha) {
	/*Colour oColour = Colour();
	for(uint8_t i=0; i<3; i++)
		*(&(oColour.r) + i) = ((uint16_t)*(&this->r + i)*(255 - alpha) + *(&topColour.r +i)*(alpha)) >> 8;
	*/

	return (*this)*(1-alpha) + topColour*alpha;
}
void Colour::overlay(Colour bottom, Colour top, float alpha) {
	for(uint8_t i=0; i<3; i++)
		(&this->r)[i] = (uint32_t((&bottom.r)[i])*(1 - alpha) + (&top.r)[i]*(alpha));
}

Colour Colour::operator +(Colour secondColour) {
	Colour oColour = *this;
	for(uint8_t i=0; i<3; i++)
		(&(oColour.r))[i] = (&secondColour.r)[i] + (&this->r)[i];

	return oColour;
}
Colour Colour::operator *(float brightness) {
	Colour oColour = *this;
	oColour.bMod(brightness);

	return oColour;
}

#define MERGE_OVERLAY(code) (this->code) = (this->code * own_transmission_p) + (top.code * (1 - own_transmission_p))
Colour& Colour::merge_overlay(const Colour &top, float alpha) {
	float total_alpha_top = top.alpha * alpha;

	if(total_alpha_top < 0.001)
		return *this;
	else if(total_alpha_top > 0.999) {
		this->r = top.r;
		this->g = top.g;
		this->b = top.b;
		this->alpha = 1;

		return *this;
	}

	float own_transmission = this->alpha * (1.0f - total_alpha_top);
	float own_transmission_p = 0;

	if(own_transmission != 0)
		own_transmission_p = own_transmission / (own_transmission + total_alpha_top);

	MERGE_OVERLAY(r);
	MERGE_OVERLAY(g);
	MERGE_OVERLAY(b);

	this->alpha = (1 - (1 - this->alpha) * (1 - total_alpha_top));

	return *this;
}

#define MERGE_MULT_Colour(code) this->code = this->code * (1 - total_alpha + total_alpha * top.code);

Colour& Colour::merge_multiply(const Colour &top, float alpha) {
	uint32_t total_alpha = top.alpha * alpha;

	MERGE_MULT_Colour(r);
	MERGE_MULT_Colour(g);
	MERGE_MULT_Colour(b);

	return *this;
}
#define MERGE_MULT_SCALAR(code) (code) = (code*scalar);
Colour& Colour::merge_multiply(float scalar) {
	MERGE_MULT_SCALAR(r);
	MERGE_MULT_SCALAR(g);
	MERGE_MULT_SCALAR(b);

	return *this;
}
Colour& Colour::merge_add(const Colour &top, float alpha) {
	uint16_t total_alpha = top.alpha * alpha;

	for(uint8_t i=0; i<3; i++) {
		float& c = (&r)[i];
		c = fminf(1, fmaxf(0, c + (&top.r)[i] * total_alpha));
	}
	this->alpha = fminf(1, this->alpha + fabsf(total_alpha));

	return *this;
}

#define T_MERGE(code) (this->code) = (top.code * alpha) + (this->code * (1-alpha))
Colour& Colour::merge_transition(const Colour &top, float alpha) {
	T_MERGE(r);
	T_MERGE(g);
	T_MERGE(b);
	T_MERGE(alpha);

	return *this;
}

Colour Colour::calculate_overlay(const Colour &top, float alpha) const {
	Colour oColour = *this;

	return (oColour.merge_overlay(top, alpha));
}
Colour Colour::calculate_multiply(const Colour &top, float alpha) const {
	Colour oColour = *this;

	return (oColour.merge_multiply(top, alpha));
}
Colour Colour::calculate_multiply(float scalar) const {
	Colour oColour = *this;

	return (oColour.merge_multiply(scalar));
}
Colour Colour::calculate_add(const Colour &top, float alpha) const {
	Colour oColour = *this;

	return (oColour.merge_add(top, alpha));
}

}
} /* namespace Peripheral */

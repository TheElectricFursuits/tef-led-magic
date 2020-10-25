
#include "tef/led/Layer.h"

namespace TEF {
namespace LED {

Layer::Layer(const int length) : colours(length) {
	alpha = 1;

	fill(0);
}
Layer::Layer(const Layer &source) : colours(source.colours) {
	alpha = source.alpha;
}
Layer::Layer(Colour colour) : Layer(1) {
	colours[0] = colour;
}

int Layer::length() const {
	return colours.size();
}

Colour& Layer::get(int id) {
	id %= length();

	if(id < 0)
		id += length();

	return colours[id];
}

Colour& Layer::operator[](int id) {
	return get(id);
}
Colour Layer::operator[](int id) const {
	id %= length();
	if(id < 0)
		id += length();

	return colours[id];
}
Layer& Layer::operator=(const Layer& source) {
	this->colours = source.colours;
	this->alpha = source.alpha;

	return *this;
}

Layer& Layer::fill(Colour fColour, int from, int to) {
	if(to == -1)
		to = length();

	if(from < 0)
		from = 0;
	if(to > length())
		to = length();

	if(from > to) {
		int temp = to;
		to = from;
		from = temp;
	}

	for(int i=from; i<to; i++)
		this->get(i) = fColour;

	return *this;
}

Layer& Layer::merge_overlay(const Layer &top, int offset, bool wrap) {
	int from = offset;
	int to   = offset + top.length();

	if(!wrap) {
		if(from < 0)
			from = 0;
		if(to > length())
			to = length();
	}

	for(int i=from; i<to; i++) {
		this->get(i).merge_overlay(top[i-offset], top.alpha);
	}

	return *this;
}
Layer& Layer::merge_multiply(const Layer &top, int offset, bool wrap) {
	int from = offset;
	int to   = offset + top.length();

	if(!wrap) {
		if(from < 0)
			from = 0;
		if(to > length())
			to = length();
	}

	for(int i=from; i<to; i++) {
		this->get(i).merge_multiply(top[i-offset], top.alpha);
	}

	return *this;
}
Layer& Layer::merge_multiply(const std::vector<uint8_t> &scalars, int offset, bool wrap) {
	int from = offset;
	int to   = offset + scalars.size();

	if(!wrap) {
		if(from < 0)
			from = 0;
		if(to > length())
			to = length();
	}


	for(int i=from; i<to; i++) {
		this->get(i).merge_multiply(scalars[i-offset]);
	}

	return *this;
}
Layer& Layer::merge_add(const Layer &top, int offset, bool wrap) {
	int from = offset;
	int to   = offset + top.length();

	if(!wrap) {
		if(from < 0)
			from = 0;
		if(to > length())
			to = length();
	}

	for(int i=from; i<to; i++) {
		this->get(i).merge_add(top[i-offset], top.alpha);
	}

	return *this;
}
Layer& Layer::merge_transition(const Layer &top, int offset, bool wrap) {
	int from = offset;
	int to   = offset + top.length();

	if(!wrap) {
		if(from < 0)
			from = 0;
		if(to > length())
			to = length();
	}

	int top_count = (from - offset) % top.length();
	if(top_count < 0)
		top_count += top.length();

	const Colour *from_ptr = &top.colours.front() + top_count;
	const Colour *from_ptr_end = &top.colours.back() + 1;
	const Colour *from_ptr_start = &top.colours.front();
	Colour *to_ptr = &this->get(from);
	Colour *to_ptr_end = &colours.back() + 1;
	Colour *to_ptr_start = &colours.front();

	for(int i=from; i<to; i++) {
		to_ptr->merge_transition(*from_ptr, top.alpha);
		if(++to_ptr >= to_ptr_end)
			to_ptr = to_ptr_start;
		if(++from_ptr >= from_ptr_end)
			from_ptr = from_ptr_start;
	}

	return *this;
}

Layer& Layer::alpha_set(const std::vector<float> &newAlphas) {
	int to = newAlphas.size();
	if(to > this->length())
		to = this->length();

	for(int i=0; i<to; i++)
		colours[i].alpha = newAlphas[i];

	return *this;
}

}
}

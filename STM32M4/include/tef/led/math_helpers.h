/*
 * math_helpers.h
 *
 *  Created on: 16 Feb 2021
 *      Author: xasin
 */

#ifndef TEF_LED_MAGIC_MATH_HELPERS_H_
#define TEF_LED_MAGIC_MATH_HELPERS_H_

#include <vector>
#include <math.h>

namespace TEF {
namespace LED {

float ease_func(float progress);

struct point_t {
	float x;
	float y;

	inline point_t  operator  +(const point_t &other) const& { return point_t({x + other.x, y + other.y}); };
	inline point_t &operator +=(const point_t &other) 		{ x += other.x; y += other.y; return *this; };
	inline point_t  operator  +(const point_t &other) && 	{ return (*this) += other; };

	inline point_t  operator  -(const point_t &other) const& { return point_t({x - other.x, y - other.y}); };
	inline point_t &operator -=(const point_t &other) 		{ x -= other.x; y -= other.y; return *this; };
	inline point_t  operator  -(const point_t &other) && 	{ return (*this) -= other; };

	inline point_t  operator  *(float scalar) const& { return point_t({x * scalar, y * scalar}); };
	inline point_t &operator *=(float scalar) 		{ x *= scalar; y *= scalar; return *this; };
	inline point_t &operator  *(float scalar) && 	{ return (*this) *= scalar; };

	inline point_t  operator  /(float scalar) const& { return point_t({x / scalar, y / scalar}); };
	inline point_t &operator /=(float scalar) 		{ x /= scalar; y /= scalar; return *this; };
	inline point_t &operator  /(float scalar) && 	{ return (*this) /= scalar; };


	inline float operator *(const point_t &other) const { return x * other.x + y * other.y; };
	inline float cross_p   (const point_t &other) const { return x * other.y - y * other.x; };

	inline point_t  round() const& 	{ return point_t({roundf(x), roundf(y)}); };
	inline point_t &round() && 		{ x = roundf(x); y = roundf(y); return *this; };
	inline point_t  floor() const& 	{ return point_t({floorf(x), floorf(x)}); };
	inline point_t &floor() && 		{ x = floorf(x); y = floorf(y); return *this; };
	inline point_t  ceil()  const& 	{ return point_t({ceilf(x), ceilf(x)}); };
	inline point_t &ceil() && 		{ x = ceilf(x); y = ceilf(y); return *this; };

	inline float length() const { return sqrtf(x*x + y*y); };

	inline point_t  norm() const& { return (*this) /  length(); };
	inline point_t& norm() &&	  { return (*this) /= length(); }

	inline point_t rotate(float fact_cos, float fact_sin) const {
		return point_t({fact_cos * x - fact_sin * y, fact_cos * y + fact_sin * x});
	};
	inline point_t rotate(float fact_cos, float fact_sin, point_t centerpoint) const {
		return ((*this) - centerpoint).rotate(fact_cos, fact_sin) + centerpoint;
	};

	point_t rotate(float angle, const point_t &centerpoint = {}) const;
};

struct line_t {
	point_t start;
	point_t end;

	inline line_t  operator  +(const point_t &other) const& { return line_t({start+other, end+other}); };
	inline line_t  operator  +(const line_t  &other) const& { return line_t({start+other.start, end+other.end}); };
	inline line_t &operator +=(const point_t &other) 		{ start += other; end += other; return *this; };
	inline line_t &operator +=(const line_t  &other)		{ start += other.start; end += other.end; return *this; };
	inline line_t &operator  +(const point_t &other) &&		{ return (*this) += other; };

	inline line_t  operator  -(const point_t &other) const&	{ return line_t({start-other, end-other}); };
	inline line_t &operator -=(const point_t &other) 		{ start -= other; end -= other; return *this; };
	inline line_t &operator  -(const point_t &other) &&		{ return (*this) -= other; };

	inline line_t  operator  *(float scalar) const& { return line_t({start * scalar, end * scalar}); };
	inline line_t &operator *=(float scalar) 		{ start *= scalar; end *= scalar; return *this; };
	inline line_t &operator  *(float scalar) &&		{ return (*this) *= scalar; };

	inline line_t  operator  /(float scalar) const&	{ return line_t({start / scalar, end / scalar}); };
	inline line_t &operator /=(float scalar) 		{ start /= scalar; end /= scalar; return *this; };
	inline line_t &operator  /(float scalar) &&		{ return (*this) /= scalar; };

	inline float min_x() const { return std::min(start.x, end.x); };
	inline float min_y() const { return std::min(start.y, end.y); };
	inline float max_x() const { return std::max(start.x, end.x); };
	inline float max_y() const { return std::max(start.y, end.y); };

	float dist_along(const point_t &other) const;
	float dist_from (const point_t &other) const;

	float length() const;

	point_t norm() const;

	inline line_t rotate(float fact_cos, float fact_sin) const {
		return line_t({start.rotate(fact_cos, fact_sin), end.rotate(fact_cos, fact_sin)});
	};
	inline line_t rotate(float fact_cos, float fact_sin, point_t centerpoint) const {
		return line_t({start.rotate(fact_cos, fact_sin, centerpoint), end.rotate(fact_cos, fact_sin, centerpoint)});
	};

	line_t rotate(float angle, point_t centerpoint = {});
};

class line_dist_calc_t {
public:
	const float   length;
	const point_t normal_direction;

	const float cross_offset;
	const float dot_offset;

	line_dist_calc_t(const line_t &line);

	float dist_along(const point_t &point) const;
	float dist_from(const point_t &point)  const;
};

class polygon_t {
protected:
	std::vector<line_t> lines;

	float length;
	point_t top_left;
	point_t bottom_right;

	void recalculate_bounds();
	void recalculate_length();

public:
	polygon_t();

	polygon_t(const polygon_t &other);
	polygon_t(polygon_t &&move);

	polygon_t(std::initializer_list<line_t>  lines);
	static polygon_t from_points(std::initializer_list<point_t> vertices);

	static polygon_t from_svg(const char *str);

	polygon_t &operator =(polygon_t &&move);
	polygon_t &operator =(const polygon_t &move);

	polygon_t  operator  +(const point_t &point) const&;
	polygon_t &operator +=(const point_t &point);
	inline polygon_t &operator +(const point_t &point) && { return (*this) += point; };

	polygon_t  operator  -(const point_t &point) const&;
	polygon_t &operator -=(const point_t &point);
	inline polygon_t &operator -(const point_t &point) && { return (*this) -= point; };

	polygon_t  operator  *(float scalar) const&;
	polygon_t  &operator *=(float scalar);
	inline polygon_t &operator *(float scalar) && { return (*this) *= scalar; };

	// Upscale to a number of LINES, not vertices!
	// Important distinction for nonmanifold polygons!
	polygon_t &upscale_inplace(int n);

	polygon_t &merge_inplace(const polygon_t &source, float amount);

	polygon_t  scale_copy(float scalar, const point_t &point) const&;
	polygon_t &scale_inplace(float scalar, const point_t &centerpoint);

	polygon_t  rotate_copy(float angle, const point_t &centerpoint) const&;
	polygon_t &rotate_inplace(float angle, const point_t &centerpoint);

	inline point_t get_top_left()		const { return top_left; };
	inline point_t get_bottom_right()	const { return bottom_right; };
	inline std::vector<line_t> const get_lines() const { return lines; };

	inline float get_length()	const { return length; };

	point_t get_point_at(float length);
};

}
}



#endif /* TEF_LED_MAGIC_MATH_HELPERS_H_ */

//
//  Math.h
//  GravityBeats
//
//  Created by Joshua Moerman on 1/12/13.
//  Copyright (c) 2013 Vadovas. All rights reserved.
//

#ifndef GravityBeats_Math_h
#define GravityBeats_Math_h

#include <cmath>

namespace math {
	struct Vec2 {
		float x, y;
		
		Vec2& operator *= (float rh){
			x *= rh;
			y *= rh;
			return *this;
		}
		
		Vec2& operator+= (Vec2 const & rh){
			x += rh.x;
			y += rh.y;
			return *this;
		}
		
		float sqr_length() const {
			return x*x + y*y;
		}
		
		float length() const {
			return std::sqrt(sqr_length());
		}
		
		Vec2& normalize(){
			const float l = length();
			if(l == 0.0f) { return *this; }
			x /= l;
			y /= l;
			return *this;
		}
	};
	
	inline Vec2 operator*(float lh, Vec2 rh){
		rh *= lh;
		return rh;
	}
	
	inline Vec2 operator+(Vec2 lh, Vec2 const & rh){
		return lh += rh;
	}
	
	inline Vec2 operator-(Vec2 lh, Vec2 const & rh){
		return lh += -1.0*rh;
	}
	
	inline float dot(Vec2 const & lh, Vec2 const & rh){
		return lh.x*rh.x + lh.y*rh.y;
	}
	
	inline Vec2 rotate_ccw(Vec2 v){
		std::swap(v.x, v.y);
		v.x *= -1.0;
		return v;
	}
	
	inline Vec2 normalize(Vec2 v){
		return v.normalize();
	}
	
	inline float distance_point_point(Vec2 p1, Vec2 p2){
		return (p2 - p1).length();
	}
	
	// p point, [v,w] line segment
	inline float distance_point_line(Vec2 p, Vec2 v, Vec2 w){
		// http://stackoverflow.com/questions/849211/shortest-distance-between-a-point-and-a-line-segment
		const float l2 = (v-w).sqr_length();
		if (l2 == 0.0) return distance_point_point(p, v);

		const float t = dot(p - v, w - v) / l2;
		if (t < 0.0) return distance_point_point(p, v);
		else if (t > 1.0) return distance_point_point(p, w);
		
		auto projection = v + t * (w - v);
		return distance_point_point(p, projection);
	}
	
	template <typename T>
	constexpr T clamp(T input, T min, T max){
		return input > max ? max :
		input < min ? min : input;
	}
}

#endif

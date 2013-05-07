//
//  Simulation.h
//  GravityBeats
//
//  Created by Joshua Moerman on 1/5/13.
//  Copyright (c) 2013 Vadovas. All rights reserved.
//

#ifndef GravityBeats_Simulation_h
#define GravityBeats_Simulation_h

#include <vector>
#include <cmath>

#include "Math.h"
#include "UserInformation.h"

// TODO: if not needed, remove forcefield

namespace simulation {

template <typename Info>
struct Ball : public UserInformation<Info> {
	math::Vec2 position{0.0, 0.0};
	math::Vec2 speed{0.0, 0.0};
	
	template <typename... S>
	Ball(float x, float y, float dx, float dy, S... args)
	: UserInformation<Info>(args...)
	, position{x, y}
	, speed{dx, dy}
	{}
};

enum LineKind {
	kFallThrough,
	kOneWay
};

template <typename Info>
struct Line : public UserInformation<Info> {
	math::Vec2 starting_point;
	math::Vec2 end_point;
	math::Vec2 normal;
	float sqr_length;
	LineKind line_kind;
	
	template <typename... S>
	Line(math::Vec2 starting_point, math::Vec2 end_point, LineKind line_kind, S... args)
	: UserInformation<Info>(args...)
	, starting_point(starting_point)
	, end_point(end_point)
	, line_kind(line_kind)
	{
		update_normal_and_length();
	}
	
	void update_normal_and_length(){
		auto dir = end_point - starting_point;
		normal = normalize(rotate_ccw(dir));
		sqr_length = dir.sqr_length();
	}
};

template <typename BallInfo, typename LineInfo>
struct CollisionData {
	Ball<BallInfo> ball;
	Line<LineInfo> line;
	
	// For this trick I need inheriting ctors... Which we dont have yet...
	//struct : public UserInformation<BallInfo> {} ball_information;
	//struct : public UserInformation<LineInfo> {} line_information;
	
	CollisionData(Ball<BallInfo> const & b, Line<LineInfo> const & l)
	: ball(b), line(l)
	{}
};

struct Bounds{
	float xmin{0};
	float xmax{1280};
	float ymin{0};
	float ymax{800};
};

template <typename BallInfo, typename LineInfo>
struct Simulation {
	typedef Ball<BallInfo> ball_type;
	typedef Line<LineInfo> line_type;
	typedef CollisionData<BallInfo, LineInfo> collision_type;
	
	std::vector<ball_type> balls;
	std::vector<line_type> lines;
	std::vector<collision_type> collisions_in_update;
	
	Bounds bounds;
	math::Vec2 gravity{0.0f, 50.0f};
	
	float collision_timer{0.0};
	int total_collisions{0};
	int collisions{0};
	int collisions_per_second{0};
	
	Simulation(){
		balls.reserve(500);
		lines.reserve(10);
		collisions_in_update.reserve(1000);
	}
	
	std::vector<collision_type> update(float dt){
		collisions_in_update.clear();
		
		// move balls
		for(auto& b : balls){		
			b.speed += dt * gravity;
			
			auto rest_time = collide(b, lines, dt);
			while(rest_time.second >= 0.0f && rest_time.first){
				rest_time = collide(b, lines, rest_time.second, rest_time.first);
			}
			b.position += rest_time.second*b.speed;
		}
		
		// count collisions per second (per half second)
		collision_timer += dt;
		if(collision_timer > 0.5){
			collision_timer -= 0.5;
			collisions_per_second = collisions * 2;
			collisions = 0.0;
		}
		
		// remove out-of-scene balls
		for(auto it = balls.begin(); it != balls.end();){
			ball_type & b = *it;
			if(b.position.y > bounds.ymax || b.position.x < bounds.xmin || b.position.x > bounds.xmax){
				it = balls.erase(it);
			} else {
				++it;
			}
		}
		
		return collisions_in_update;
	}
	
	/*
	 Part about the collisions
	 */
	float collision_time(ball_type const & b, line_type const & l){
		// -dot(b.pos - l.start) / ...
		return dot(l.starting_point - b.position, l.normal) / dot(b.speed, l.normal);
	}
	
	std::pair<bool, math::Vec2> check_collision(ball_type const & b, line_type const & l, float dt){
		auto t = collision_time(b, l);
		// does it collide within the given time? AND does it go the right way?
		if(0 <= t && t <= dt){
			if(l.line_kind == kOneWay && dot(b.speed, l.normal) > 0.0){
				return {false, {0,0}};
			}
			// does it collide with the finite line?
			auto collision = b.position + t*b.speed;
			auto on_line = dot(collision - l.starting_point, l.end_point - l.starting_point);
			if(0 <= on_line && on_line <= l.sqr_length){
				return {true, collision};
			} else {
				return {false, {0,0}};
			}
		} else {
			return {false, {0,0}};
		}
	}
	
	std::pair<line_type const *, float> collide(ball_type& b, std::vector<line_type> const & lines, float dt, line_type const * ignore = nullptr){
		line_type const * closest_line = nullptr;
		math::Vec2 collision{0.0, 0.0};
		float closeness = 100.0f;
		
		for(auto& l : lines){
			if(&l == ignore) continue;
			auto ret = check_collision(b, l, dt);
			if(ret.first){
				auto t = collision_time(b, l);
				
				if(0 <= t && t < closeness){
					closest_line = &l;
					closeness = t;
					collision = ret.second;
				}
			}
		}
		
		if(closest_line == nullptr){
			return std::make_pair(closest_line, dt);
		} else {
			++total_collisions;
			++collisions;
			
			b.position = collision;
			auto const & l = *closest_line;
			
			collisions_in_update.emplace_back(b, l);
			
			if(l.line_kind != kFallThrough){
				auto b1 = -dot(b.speed, l.normal) * l.normal;
				b.speed = 2.0*b1 + b.speed;
			}
			
			return std::make_pair(closest_line, dt - closeness);
		}
	}
};

}

#endif

//
//  Beat.h
//  GravityBeats
//
//  Created by Joshua Moerman on 1/9/13.
//  Copyright (c) 2013 Vadovas. All rights reserved.
//

#ifndef GravityBeats_Beat_h
#define GravityBeats_Beat_h

#include <vector>
#include <cmath>

#include "UserInformation.h"

template <typename Information>
struct Note : public UserInformation<Information>{
	// see http://en.wikipedia.org/wiki/Note_value
	enum Speed {
		kWholeNote	=1,	// 1 in one measure
		kHalfNote	=2,	// 2 in one measure
		kTriplet	=3,	// 3 in one measure
		kQuarterNote	=4,	// 4
		kHalfTriplet	=6,	// 6
		kEighthNote	=8	// 8
	} speed;
	int progress = -2;
	
	template <typename... S>
	Note(Speed speed, S... args)
	: UserInformation<Information>(args...)
	, speed(speed)
	{}
	
	// we could return how much it is off, to give the balls the right initial position and speed.
	bool update(float time_in_measure){
		int new_progress = std::floor(time_in_measure*speed);
		if(new_progress != progress){
			if(progress == -2){
				progress = new_progress;
				return false;
			} else {
				progress = new_progress;
				return true;
			}
		}
		return false;
	}
	
	void reset(){
		progress = -1;
	}
};

template <typename NoteInformation>
struct Beat {
	float total_length{4.0};		// 60 bpm
	float time{-0.1};
	
	std::vector<Note<NoteInformation>> notes;
	
	std::vector<NoteInformation> update(float dt){
		std::vector<NoteInformation> ret;
		time += dt;
		
		// not needed, but keeps the floats small
		// also usefull when we want to change bpm
		if(time > total_length){
			time -= total_length;
			for(auto& n : notes){
				n.reset();
			}
		}
		
		for(auto& n : notes){
			if(n.update(time / total_length)){
				ret.push_back(n.information);
			}
		}
		
		return ret;
	}
};

#endif

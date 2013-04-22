//
//  Scales.h
//  GravityBeats
//
//  Created by Joshua Moerman on 1/25/13.
//  Copyright (c) 2013 Vadovas. All rights reserved.
//

#ifndef GravityBeats_Scales_h
#define GravityBeats_Scales_h

#include <vector>
#include <cmath>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cassert>

// http://www.phys.unsw.edu.au/jw/notes.html

inline float pitch_for_midi_note(int note){
	// we say 440hz is 1 in our units
	float exponent = (note - 69) / 12.0f;
	return std::pow(2.0f, exponent);
}

struct Scale {
	std::vector<int> notes;
	
	int note_for_length(float length){
		assert(notes.size() > 1);

		// determine note
		length /= 200.0;
		float note = -std::log(length) / std::log(2.0f) * 12.0f + 69.0f;
		
		// determine note in scale
		auto it = notes.begin();
		while(*it < note){
			++it;
			if(it == notes.end()) {
				--it;
				break;
			}
		}
		
		// determine closest note in scale
		if(it == notes.begin()){
			return *it;
		} else {
			auto it2 = it - 1;
			if(std::abs(*it - note) > std::abs(*it2 - note)){
				return *it2;
			} else {
				return *it;
			}
		}
	}
	
	static Scale load_from_file(std::string filename){
		std::ifstream file(filename);
		if(!file) throw std::runtime_error("Couldn't open file " + filename);
		
		Scale scale;
		
		int note = 0;
		while (file >> note) {
			scale.notes.push_back(note);
		}
		
		std::sort(scale.notes.begin(), scale.notes.end());
		
		return scale;
	}
};

#endif

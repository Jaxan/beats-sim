Gravity Beats simulation
========================

This is one of the components of the Gravity Beats app (both the iPad app, and the multi-client app). These are all the things needed for the simulation, generation of balls and pitch of sounds. The idea behind the physics is to simply do a recursive raytrace in one simulation step, so that it is possible for a ball to bounce multiple times in one time-frame. Only linear interpolation is used in a single simulation step.

A simple usage may look like this:

``` C++
#include "Simulation.h"
#include "Beat.h"
using namespace simulation;

// This can be used to provide information in balls/lines
// For example, in the original app:
// ball_info would be: Color
// line_info would be: std::shared_ptr<Sound>
using ball_info = void;
using line_info = void;
using simu_type = Simulation<ball_info, line_info>;

// Notes are used to add balls, so we need a position
using note_info = Vec2;
using note_type = Note<note_info>;
using beat_type = Beat<note_info>;

int main(){
	simu_type sim;
	beat_type beat;
	
	// Create one beat, and one line
	beat.notes.emplace_back(note_type::kQuarterNote, note_info{100.0f, 100.0f});
	sim.lines.empace_back(Vec2{50.0f, 100.0f}, Vec2{150.0f, 200.0f}, kOneWay);
	
	while(true){
		float dt = 1.0 / 60.0;
		for(auto collision : sim.update(dt)){
			// Do something, eg:
			// collision.line.information->play();
		}

		for(auto n : online_sim.beat.update(dt)){
			// Add ball:
			sim.balls.emplace_back(n.x, n.y, 0, 0);
		}		
	}
}
```


Design rationale
================

For me this was also a playground to try out a different design than what I was used to. Normally I would use callbacks for collisions (just pass a function to the simulation which would be called). Now this has the disadvatage that we call those callback *while* iterating over the balls (and lines), this implies that we cannot update the balls/lines in the callback (or we should copy them before iterating). So I choose to return all collisions as a result of one update (note that one could also make a public member ```collisions```, with the latest collisions, but this is weird). This design makes it (theoretically) also easier to multithread it, because we know, once we call ```update```, only the simulation will run, and the collected collisions can also be processed in parallel.

Another new thing I tried is the information inside a ball and line. I've seen (for example in Bullet) that it is common to provide a ```void* userInformation``` in objects handled by another system (eg. a physics simulation). This of course is not type-safe, requires allocation on the heap (or other guarantees about life-time) and always needs a couple of bytes (probably 8, depending on the OS). Templating this makes it type-safe, makes it possible to use automatic variable (without worrying about them), and can use smaller types (eg. if we only need a bool), while preserving the original flexibility, because we can use ```T = void*```. Note that it is optimized for ```T = void``` to (depending on the compiler) use no space at all. For me this worked remarkably well, but all the templating makes it a bit more verbose (that's why I have all those ```typedef```s).

Eventually this made it easy to store both the lines and balls in one contiguous array. This makes it for example possible to send exactly that array to OpenGL, by just setting the right stride. Hence no copies are needed and you can draw all the balls with only one draw call :).


Other parts of Gravity Beats
============================

I am planning to also open source the server used by the multi-client version, because it is also nice to see a websockets example and JSON serialisation with boost fusion. But for now, this is all.

Original app: https://vimeo.com/60401549

Multi-client app: http://vadovas-studios.com/beats/


Licence
=======

Copyright (c) 2013 Joshua Moerman, Vadovas. All rights reserved.

TL;DR: In any usage please provide my name and company name ;).


Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

INSERT BLABLA IN CAPS. PROVIDED "AS IS" ETC ETC.

	

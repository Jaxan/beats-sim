//
//  UserInformation.h
//  GravityBeats
//
//  Created by Joshua Moerman on 2/19/13.
//  Copyright (c) 2013 Vadovas. All rights reserved.
//

#ifndef GravityBeats_UserInformation_h
#define GravityBeats_UserInformation_h

template <typename T>
struct UserInformation {
	template <typename... S>
	UserInformation(S... args)
	: information(args...)
	{}
	
	T information;
};

template <>
struct UserInformation<void> {};

#endif

//
// Copyright (c) 2018 Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
//

#pragma once

////////////////////////////////////////

#include <Carbon/Carbon.h>
#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#else
using id = void *;
//template <typename T> struct id 
//{
//	explicit id(T *p) _x( p ) {}
//	void *p;
//};
#endif

//
// Copyright (c) 2016 Gecko project
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
// OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#pragma once

#include <vector>
#include <memory>
#include "waitable.h"

namespace platform
{

////////////////////////////////////////

/// @brief Abstract dispatcher.
///
/// A dispatcher will send out events when executed.
/// Calling exit will cause the execute method to return with the given code.
/// Events are sent out by other classes (keyboard, mouse, window, or timer).
class dispatcher
{
public:
	/// @brief Constructor.
	dispatcher( void );

	/// @brief Destructor.
	virtual ~dispatcher( void );

	/// @brief Execute the dispatcher.
	///
	/// The dispatcher will begin sending out events until the exit() method is called.
	/// @return The exit code
	virtual int execute( void ) = 0;

	/// @brief Cause execute to stop.
	///
	/// The execute method will return the given code.
	/// @param code Exit code
	virtual void exit( int code ) = 0;

	/// @brief registers a waitable object
	///
	/// execution loops are expected to pay attention to these
	virtual void add_waitable( const std::shared_ptr<waitable> &w );

	/// @brief unregisters a waitable object
	///
	/// execution loops are expected to pay attention to these
	virtual void remove_waitable( const std::shared_ptr<waitable> &w );

protected:
	std::vector<std::shared_ptr<waitable>> _waitables;
};

////////////////////////////////////////

}


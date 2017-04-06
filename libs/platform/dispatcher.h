//
// Copyright (c) 2013-2017 Ian Godin and Kimball Thurston
// All rights reserved.
// Copyrights licensed under the MIT License.
// See the accompanying LICENSE.txt file for terms
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


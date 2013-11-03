
#pragma once

namespace platform
{

////////////////////////////////////////

/// @brief Abstract dispatcher class
/// A dispatcher will send out events when executed.
/// Calling exit will cause the execute method to return with the given code.
/// Events are sent out by other classes (keyboard, mouse, window, or timer).
class dispatcher
{
public:
	/// @brief Constructor
	dispatcher( void );

	/// @brief Destructor
	virtual ~dispatcher( void );

	/// @brief Execute the dispatcher
	/// The dispatcher will begin sending out events until the exit() method is called.
	/// @return The exit code
	virtual int execute( void ) = 0;

	/// @brief Cause execute to stop.
	/// The execute method will return the given code.
	/// @param code Exit code
	virtual void exit( int code ) = 0;
};

////////////////////////////////////////

}


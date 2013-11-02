
#pragma once

#include <memory>
#include "builder.h"
#include "window.h"

namespace gui
{
	class style;

////////////////////////////////////////

class application : public std::enable_shared_from_this<application>
{
public:
	application( void );
	~application( void );

	std::shared_ptr<window> new_window( void );

	void set_style( const std::shared_ptr<style> &sty ) { _style = sty; }
	const std::shared_ptr<style> &get_style( void ) const { return _style; }

	int run( void );
	void exit( int code );

	void push( void );
	void pop( void );

	std::shared_ptr<draw::font> get_font( const std::string &family, const std::string &style, double pixsize );

	static std::shared_ptr<application> current( void );

private:
	struct impl;
	std::unique_ptr<impl> _impl;

	std::shared_ptr<style> _style;
};

////////////////////////////////////////

}


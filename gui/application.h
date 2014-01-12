
#pragma once

#include <memory>
#include <set>
#include "window.h"

namespace gui
{
	class style;

////////////////////////////////////////

class application : public std::enable_shared_from_this<application>
{
public:
	application( const std::string &platform = std::string(), const std::string &render = std::string() );
	~application( void );

	const std::string &active_platform( void ) { return _platform; }

	std::shared_ptr<window> new_window( void );

	void set_style( const std::shared_ptr<style> &sty ) { _style = sty; }
	const std::shared_ptr<style> &get_style( void ) const { return _style; }

	int run( void );
	void exit( int code );

	void push( void );
	void pop( void );

	std::set<std::string> get_font_families( void );
	std::set<std::string> get_font_styles( const std::string &family );
	std::shared_ptr<gldraw::font> get_font( const std::string &family, const std::string &style, double pixsize );

	static std::shared_ptr<application> current( void );
	static const std::shared_ptr<style> &current_style( void );

private:
	struct impl;
	std::unique_ptr<impl> _impl;

	std::shared_ptr<style> _style;
	std::string _platform;
};

////////////////////////////////////////

}


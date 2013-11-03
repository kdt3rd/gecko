
#pragma once

#include <string>
#include <set>
#include <memory>
#include <draw/font.h>

namespace platform
{

////////////////////////////////////////

/// @brief Font manager
///
/// Allows creation and listing of fonts.
class font_manager
{
public:
	/// @brief Constructor.
	font_manager( void );

	/// @brief Destructor.
	virtual ~font_manager( void );

	/// @brief Name of font manager.
	///
	/// The name of the font manager being used.
	/// @return The name of the font manager
	const std::string &name( void ) const { return _name; }

	/// @brief Version of the font manager.
	///
	/// The version of the font manager being used.
	/// @return The version of the font manager
	const std::string &version( void ) const { return _version; }


	/// @brief List of families.
	///
	/// Get a list of families of all managed fonts.
	/// @return The list of families
	virtual std::set<std::string> get_families( void ) = 0;

	/// @brief List of style.
	///
	/// Get a list of styles available.
	/// @return The list of styles.
	virtual std::set<std::string> get_styles( void ) = 0;

	/// @brief Get a particular font.
	///
	/// Create and return a font with the given attributes.
	/// @param family Family of the font to create
	/// @param style Style of the font to create
	/// @param pixsize Size of the font to create
	///
	/// @return The best matching font found
	virtual std::shared_ptr<draw::font> get_font( const std::string &family, const std::string &style, double pixsize ) = 0;

protected:
	void set_manager_name( std::string s ) { _name = std::move( s ); }
	void set_manager_version( std::string v ) { _version = std::move( v ); }

	std::string _name;
	std::string _version;
};

////////////////////////////////////////

}


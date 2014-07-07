
#pragma once

#include <memory>
#include <base/alignment.h>
#include <base/gradient.h>
#include <base/paint.h>
#include <base/path.h>
#include <base/rect.h>
#include <gl/context.h>
#include <gl/program.h>
#include <script/font.h>

namespace draw
{

////////////////////////////////////////

/// @brief Canvas to draw on
class canvas : public gl::context
{
public:

	/// @brief Constructor
	canvas( void );

	/// @brief Destructor
	virtual ~canvas( void );

	std::shared_ptr<gl::texture> gradient( const base::gradient &g, size_t n = 128 );

	std::shared_ptr<gl::program> program( const std::string &vert, const std::string &frag );


	virtual base::point align_text( const std::shared_ptr<script::font> &font, const std::string &utf8, const base::rect &rect, base::alignment a );

	virtual void draw_text( const std::shared_ptr<script::font> &font, const base::point &p, const std::string &utf8, const base::paint &c );

/*
	/// @brief Fill the entire canvas
	/// @param c Color to fill with
	void fill( const base::color &c );

	/// @brief Fill the entire canvas
	/// @param p Paint to fill with
	virtual void fill( const paint &p ) = 0;

	/// @brief Fill the entire canvas
	/// @param p Paint to fill with
	virtual void fill( const base::rect &r, const paint &p ) = 0;

	/// @brief Draw a path
	/// Draw a path on the canvas
	/// @param p Path to draw
	/// @param c Paint to apply to the path
	virtual void draw_path( const base::path &p, const paint &c ) = 0;

	/// @brief Draw text
	/// Draw text on the canvas
	/// @param font The font to draw with
	/// @param p The start of the baseline
	/// @param utf8 The text to draw
	/// @param c The paint to color with
	virtual void translate( double x, double y ) = 0;

	virtual void clip( const base::rect &r ) = 0;

	/// @brief Present the canvas
	/// Finalize the drawing of the canvas and display it.
	virtual void present( void );
*/
	void save( void );
	void restore( void );

//	virtual void screenshot_png( const char *filename ) = 0;
private:
	std::shared_ptr<gl::program> _text_program;
	std::shared_ptr<gl::buffer<float>> _text_texture_vertices;
	std::shared_ptr<gl::buffer<float>> _text_output_vertices;
	std::shared_ptr<gl::buffer<uint16_t>> _text_indices;
	std::shared_ptr<gl::vertex_array> _text_array;
	std::vector<float> _text_coord_buf;
	std::vector<float> _text_texcoord_buf;
	std::vector<uint16_t> _text_idx_buf;
	struct GlyphPack
	{
		uint32_t version;
		std::shared_ptr<gl::texture> texture;
	};
	std::map<std::shared_ptr<script::font>, GlyphPack> _font_glyph_cache;
};

////////////////////////////////////////

}


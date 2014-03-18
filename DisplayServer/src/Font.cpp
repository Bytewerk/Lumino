#include <ft2build.h>
#include FT_FREETYPE_H

#include "Bitmap.h"
#include "Exception.h"
#include "Logger.h"

#include "Font.h"

#include <iostream>
using namespace std;

Font::Font(FT_Library *ftlib, const char *filename, unsigned size)
	: m_ftlib(ftlib), m_size(size)
{
	// load the font object
	int fterror;

	fterror = FT_New_Face(*ftlib, filename, 0, &m_face);
	if(fterror) {
		throw FreeTypeException("Font", "Could not load font from file", fterror);
	}

	LOG(Logger::LVL_DEBUG, "Font", "Loaded font successfully from: %s", filename);
	LOG(Logger::LVL_DEBUG, "Font", "Number of glyphs: %i", m_face->num_glyphs);

	fterror = FT_Set_Pixel_Sizes(m_face, 0, size);
	if(fterror) {
		throw FreeTypeException("Font", "Could not set the font size", fterror);
	}
}

Font::~Font()
{
}

void Font::renderText(const std::string &text, Bitmap *bitmap)
{
	int fterror;

	fterror = FT_Load_Glyph(m_face, 0x65 /*e*/, FT_LOAD_DEFAULT);
	if(fterror) {
		throw FreeTypeException("Font", "Could not load glyph", fterror);
	}

	fterror = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_MONO);
	if(fterror) {
		throw FreeTypeException("Font", "Could not render the glyph", fterror);
	}

	FT_Bitmap ftbmp = m_face->glyph->bitmap;

	LOG(Logger::LVL_DEBUG, "Font", "Bitmap size: %ix%i", ftbmp.width, ftbmp.rows);

	for(int y = 0; y < ftbmp.rows; y++) {
		for(int x = 0; x < ftbmp.width; x++) {
			unsigned char *row = ftbmp.buffer + y*ftbmp.pitch;

			unsigned byte = x / 8;
			unsigned bit  = x % 8;

			cout << ((row[byte] & (0x80 >> bit)) ? '#' : '-');
		}

		cout << endl;
	}
}

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

void Font::renderText(const std::wstring &text, Bitmap *bitmap)
{
	int fterror;
	int pen_x = 0, pen_y = 10;
	wchar_t c;

	// create a temporary bitmap with sufficient size for the whole text
	Bitmap tmpBitmap(m_size * text.length(), 3 * m_size / 2);

	for(std::string::size_type i = 0; i < text.length(); i++) {
		c = text[i];

		fterror = FT_Load_Char(m_face, c, FT_LOAD_RENDER | FT_LOAD_MONOCHROME);
		if(fterror) {
			throw FreeTypeException("Font", "Could not and render the glyph", fterror);
		}

		FT_Bitmap ftbmp = m_face->glyph->bitmap;

		// copy rendered glyph to position
		for(int y = 0; y < ftbmp.rows; y++) {
			for(int x = 0; x < ftbmp.width; x++) {
				unsigned char *row = ftbmp.buffer + y*ftbmp.pitch;

				unsigned byte = x / 8;
				unsigned bit  = x % 8;

				bool enable = (row[byte] & (0x80 >> bit)) != 0;

				unsigned outx = pen_x + m_face->glyph->bitmap_left + x;
				unsigned outy = pen_y - m_face->glyph->bitmap_top + y;

				tmpBitmap.setPixel(outx, outy, enable);
			}
		}

		pen_x += m_face->glyph->advance.x >> 6;
		pen_y += m_face->glyph->advance.y >> 6;
	}

	// determine the actual dimensions of the text
	unsigned min_x = 0xFFFFFFFF, min_y = 0xFFFFFFFF, max_x = 0, max_y = 0;

	for(unsigned x = 0; x < tmpBitmap.getWidth(); x++) {
		for(unsigned y = 0; y < tmpBitmap.getHeight(); y++) {
			if(tmpBitmap.getPixel(x, y)) {
				if(x < min_x) min_x = x;
				if(y < min_y) min_y = y;
				if(x > max_x) max_x = x;
				if(y > max_y) max_y = y;
			}
		}
	}

	// resize the output bitmap and copy the text to it
	bitmap->copyRectFromBitmap(tmpBitmap, min_x, min_y, max_x-min_x+1, max_y-min_y+1);
}

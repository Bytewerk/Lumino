#ifndef FONT_H
#define FONT_H

#include <string>

#include <ft2build.h>

class Bitmap;

class Font {
	private:
		FT_Library *m_ftlib;
		FT_Face     m_face;
		unsigned    m_size;

	public:
		Font(FT_Library *ftlib, const char *filename, unsigned size);
		~Font();

		void renderText(const std::wstring &text, Bitmap *bitmap);
};

#endif // FONT_H

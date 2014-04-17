#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

#include "Font.h"
#include "Bitmap.h"

class Framebuffer : public Bitmap
{
	public:
		static const int OVERSCROLL = 20;
		static const int LINEHEIGHT = 11;

		struct TextArea {
			int x, y;
			unsigned w, h;

			TextArea()
			{
				clear();
			}

			void clear(void)
			{
				w = h = 0;
			}

			bool isClear(void) const
			{
				return (w == 0) && (h == 0);
			}
		};

	private:
		struct TextArea m_textArea;

		Font         *m_font;
		Bitmap       *m_textBitmaps[2];
		std::wstring  m_texts[2];
		int           m_textOffsets[2];

	public:
		static const unsigned WIDTH = 160;
		static const unsigned HEIGHT = 24;

		Framebuffer(FT_Library *ftlib);
		~Framebuffer();

		void serialize(std::string *buffer) const;

		void drawText(int x, int y, const std::wstring &text);

		void shiftText(void);
		void redrawText(void);
		void setText(unsigned line, const std::wstring &text);

		void setTextArea(int x, int y, unsigned w, unsigned h);
		const struct TextArea* getTextArea() const { return &m_textArea; }
};

#endif // FRAMEBUFFER_H

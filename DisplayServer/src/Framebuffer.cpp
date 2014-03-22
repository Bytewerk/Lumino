#include <sstream>

#include "Exception.h"
#include "Logger.h"

#include "Framebuffer.h"

Framebuffer::Framebuffer(FT_Library *ftlib)
	: Bitmap(Framebuffer::WIDTH, Framebuffer::HEIGHT)
{
	this->clear(false);

	m_textBitmaps[0] = new Bitmap(0, 0);
	m_textBitmaps[1] = new Bitmap(0, 0);
	m_font = new Font(ftlib, "/usr/share/fonts/dejavu/DejaVuSans.ttf", 10);
}

Framebuffer::~Framebuffer()
{
	delete m_textBitmaps[0];
	delete m_textBitmaps[1];
	delete m_font;
}

void Framebuffer::serialize(std::string *buffer) const
{
	std::ostringstream oss;

	oss.write("\x1B\x02", 2); // start sequence

	for(Bitmap::BitmapData::size_type i = 0; i < (WIDTH*HEIGHT/8); i++) {
		if(m_data[i] == 0x1B) {
			// duplicate escape character
			oss.write("\x1B", 1);
		}

		oss.write(reinterpret_cast<const char*>(&(m_data[i])), 1);
	}

	oss.write("\x1B\x03", 2); // end sequence

	*buffer = oss.str();
}

void Framebuffer::drawText(int x, int y, const std::wstring &text)
{
	Bitmap textBitmap(0, 0);
	textBitmap.clear(false);
	m_font->renderText(text, &textBitmap);

	this->blit(textBitmap, x, y);
}

void Framebuffer::setTextArea(int x, int y, unsigned w, unsigned h)
{
	m_textArea.x = x;
	m_textArea.y = y;
	m_textArea.w = w;
	m_textArea.h = h;
}

void Framebuffer::setText(unsigned line, const std::wstring &text)
{
	if(line > 1) {
		throw RangeException("Framebuffer", "Line is out of range", line, 1);
	}

	if(m_texts[line] != text) {
		m_font->renderText(text, m_textBitmaps[line]);
		m_texts[line] = text;
		m_textOffsets[line] = OVERSCROLL;
	}
}

void Framebuffer::redrawText(void)
{
	if(m_textArea.isClear()) {
		return;
	}

	// clear the text area
	for(int dx = 0; dx < m_textArea.w; dx++) {
		for(int dy = 0; dy < m_textArea.w; dy++) {
			this->setPixel(m_textArea.x + dx, m_textArea.y + dy, 0);
		}
	}

	// render the text
	for(int line = 0; line < 2; line++) {
		Bitmap *bmp = m_textBitmaps[line];

		int sx;
		if(m_textOffsets[line] >= 0) {
			sx = 0;
		} else if(m_textOffsets[line] < ((int)m_textArea.w - (int)bmp->getWidth())) {
			sx = (int)m_textArea.w - (int)bmp->getWidth();
		} else {
			sx = m_textOffsets[line];
		}

		int miny = LINEHEIGHT * line;
		int maxy = LINEHEIGHT * (line + 1);

		if(maxy > m_textArea.h) {
			maxy = m_textArea.h;
		}

		for(int dx = 0; dx < m_textArea.w; dx++) {
			for(int dy = miny; dy < maxy; dy++) {
				int x = m_textArea.x + dx;
				int y = m_textArea.y + dy;

				this->setPixel(x, y, bmp->getPixel(dx - sx, dy - miny));
			}
		}
	}
}

void Framebuffer::shiftText(void)
{
	for(int line = 0; line < 2; line++) {
		Bitmap *bmp = m_textBitmaps[line];

		if(bmp->getWidth() > m_textArea.w) {
			m_textOffsets[line]--;

			if(m_textOffsets[line] < ((int)m_textArea.w - (int)bmp->getWidth() - OVERSCROLL)) {
				m_textOffsets[line] = OVERSCROLL;
			}
		}
	}
}

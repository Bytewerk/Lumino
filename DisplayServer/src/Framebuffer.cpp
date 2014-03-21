#include <sstream>

#include "Framebuffer.h"

Framebuffer::Framebuffer()
	: Bitmap(Framebuffer::WIDTH, Framebuffer::HEIGHT)
{
	this->clear(false);
}

Framebuffer::~Framebuffer()
{
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

void Framebuffer::setTextArea(unsigned x, unsigned y, unsigned w, unsigned h)
{
	m_textArea.x = x;
	m_textArea.y = y;
	m_textArea.w = w;
	m_textArea.h = h;
}

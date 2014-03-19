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

void Framebuffer::serialize(std::string *buffer)
{
	std::ostringstream oss;

	oss.write("\x1B\x02", 2); // start sequence

	for(Bitmap::BitmapData::size_type i = 0; i < (WIDTH*HEIGHT/8); i++) {
		if(m_data[i] == 0x1B) {
			// duplicate escape character
			oss.write("\x1B", 1);
		}

		oss.write(reinterpret_cast<char*>(&(m_data[i])), 1);
	}

	oss.write("\x1B\x03", 2); // end sequence

	*buffer = oss.str();
}

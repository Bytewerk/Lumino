#include <iostream>

#include "Bitmap.h"

using namespace std;

Bitmap::Bitmap(unsigned width, unsigned height)
	: m_width(width), m_height(height)
{
	resizeData();
}

Bitmap::~Bitmap()
{
}

void Bitmap::resizeData(void)
{
	m_data.resize(m_width * m_height / 8 + 1);
}

void Bitmap::setSize(unsigned width, unsigned height)
{
	m_width = width;
	m_height = height;
	resizeData();
}

void Bitmap::debugPrint(void) const
{
	for(int y = 0; y < m_height; y++) {
		for(int x = 0; x < m_width; x++) {
			cout << (getPixel(x, y) ? '#' : '-');
		}

		cout << endl;
	}
}

void Bitmap::extractRectFromBitmap(const Bitmap &src, unsigned x, unsigned y, unsigned w, unsigned h)
{
	this->setSize(w, h);

	for(unsigned px = 0; px < w; px++) {
		for(unsigned py = 0; py < h; py++) {
			this->setPixel(px, py, src.getPixel(x+px, y+py));
		}
	}
}

void Bitmap::clear(bool enable)
{
	uint8_t c;

	if(enable) {
		c = 0xFF;
	} else {
		c = 0x00;
	}

	for(BitmapData::size_type i = 0; i < m_data.size(); i++) {
		m_data[i] = c;
	}
}

void Bitmap::blit(const Bitmap &src, unsigned x, unsigned y)
{
	for(unsigned px = 0; px < src.getWidth(); px++) {
		for(unsigned py = 0; py < src.getHeight(); py++) {
			this->setPixel(x+px, y+py, src.getPixel(px, py));
		}
	}
}

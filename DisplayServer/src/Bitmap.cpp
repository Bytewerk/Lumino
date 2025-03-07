#include <iostream>

#include "Exception.h"

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
	if(((m_width * m_height) % 8) == 0) {
		// if pixel count is divisible by 8, data fits perfectly into bytes.
		m_data.resize(m_width * m_height / 8);
	} else {
		// if pixel count is not divisible by 8, an additional byte is needed.
		// example: 3x6 bitmap -> 18 pixels / 8 = 2 -> 2 + 1 bytes of storage are
		// required for 18 bits
		m_data.resize(m_width * m_height / 8 + 1);
	}
}

void Bitmap::setSize(unsigned width, unsigned height)
{
	m_width = width;
	m_height = height;
	resizeData();
}

void Bitmap::setData(const std::string &data) {
	if(data.size() < m_data.size()) {
		throw RangeException("Bitmap", "Data size is too small.", data.size(), m_data.size());
	}

	for(BitmapData::size_type i = 0; i < m_data.size(); i++) {
		m_data[i] = data[i];
	}
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

void Bitmap::extractRectFromBitmap(const Bitmap &src, int x, int y, unsigned w, unsigned h)
{
	this->setSize(w, h);

	for(int px = 0; px < w; px++) {
		for(int py = 0; py < h; py++) {
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

void Bitmap::blit(const Bitmap &src, int x, int y)
{
	for(int px = 0; px < src.getWidth(); px++) {
		for(int py = 0; py < src.getHeight(); py++) {
			this->setPixel(x+px, y+py, src.getPixel(px, py));
		}
	}
}

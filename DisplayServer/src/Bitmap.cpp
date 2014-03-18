#include "Bitmap.h"

Bitmap::Bitmap(unsigned width, unsigned height)
	: m_width(width), m_height(height)
{
	resizeData();
}

void Bitmap::resizeData(void)
{
	m_data.resize(m_width * m_height / 8);
}

void Bitmap::setSize(unsigned width, unsigned height)
{
	m_width = width;
	m_height = height;
	resizeData();
}

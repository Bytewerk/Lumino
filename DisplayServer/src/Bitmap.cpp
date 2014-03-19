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
	m_data.resize(m_width * m_height / 8);
}

void Bitmap::setSize(unsigned width, unsigned height)
{
	m_width = width;
	m_height = height;
	resizeData();
}

void Bitmap::debugPrint(void)
{
	for(int y = 0; y < m_height; y++) {
		for(int x = 0; x < m_width; x++) {
			cout << (getPixel(x, y) ? '#' : '-');
		}

		cout << endl;
	}
}

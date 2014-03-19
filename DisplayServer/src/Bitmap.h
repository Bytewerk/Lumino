#ifndef BITMAP_H
#define BITMAP_H

#include <cstdint>

#include <vector>

class Bitmap {
	private:
		typedef    std::vector<uint8_t> BitmapData;

		unsigned   m_width, m_height;
		BitmapData m_data;

		void resizeData(void);

	public:
		Bitmap(unsigned width, unsigned height);
		~Bitmap();

		void setSize(unsigned width, unsigned height);

		bool setPixel(unsigned x, unsigned y, bool enable)
		{
			if(x < m_width && y < m_height) {
				unsigned bit = (y * m_width + x);
				unsigned byte = bit / 8;
				bit %= 8;

				if(enable) {
					m_data[byte] |= (0x80 >> bit);
				} else {
					m_data[byte] &= ~(0x80 >> bit);
				}

				return true;
			} else {
				return false;
			}
		}

		bool getPixel(unsigned x, unsigned y)
		{
			if(x < m_width && y < m_height) {
				unsigned bit = (y * m_width + x);
				unsigned byte = bit / 8;
				bit %= 8;

				return (m_data[byte] & (0x80 >> bit)) != 0;
			} else {
				return false;
			}
		}

		void debugPrint(void);
};

#endif // BITMAP_H

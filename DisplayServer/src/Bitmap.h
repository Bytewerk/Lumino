#ifndef BITMAP_H
#define BITMAP_H

#include <cstdint>

#include <vector>

class Bitmap {
	protected:
		typedef    std::vector<uint8_t> BitmapData;

		unsigned   m_width, m_height;
		BitmapData m_data;

		void resizeData(void);

	public:
		Bitmap(unsigned width, unsigned height);
		~Bitmap();

		virtual void setSize(unsigned width, unsigned height);

		virtual bool setPixel(unsigned x, unsigned y, bool enable)
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

		virtual bool getPixel(unsigned x, unsigned y) const
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

		/*!
		 * Copy data from a rectangular region in src to this Bitmap. The size of
		 * this Bitmap is set to the size of the rectangle.
		 */
		virtual void extractRectFromBitmap(const Bitmap &src, unsigned x, unsigned y, unsigned w, unsigned h);

		/*!
		 * Copy another Bitmap to the current Bitmap at position (x,y).
		 */
		virtual void blit(const Bitmap &src, unsigned x, unsigned y);

		virtual void clear(bool enable);

		virtual unsigned getWidth() const { return m_width; }
		virtual unsigned getHeight() const { return m_height; }

		virtual void debugPrint(void) const;
};

#endif // BITMAP_H

#ifndef BITMAP_H
#define BITMAP_H

#include <cstdint>

#include <vector>
#include <string>

class Bitmap {
	protected:
		typedef    std::vector<uint8_t> BitmapData;

		unsigned   m_width, m_height;
		BitmapData m_data;

		void resizeData(void);

	public:
		Bitmap(unsigned width, unsigned height);
		virtual ~Bitmap();

		virtual void setSize(unsigned width, unsigned height);

		virtual bool setPixel(int x, int y, bool enable)
		{
			if(x >= 0 && y >= 0 && x < m_width && y < m_height) {
				int bit = (y * m_width + x);
				int byte = bit / 8;
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

		virtual bool getPixel(int x, int y) const
		{
			if(x >= 0 && y >= 0 && x < m_width && y < m_height) {
				int bit = (y * m_width + x);
				int byte = bit / 8;
				bit %= 8;

				return (m_data[byte] & (0x80 >> bit)) != 0;
			} else {
				return false;
			}
		}

		virtual void setData(const std::string &data);

		/*!
		 * Copy data from a rectangular region in src to this Bitmap. The size of
		 * this Bitmap is set to the size of the rectangle.
		 */
		virtual void extractRectFromBitmap(const Bitmap &src, int x, int y, unsigned w, unsigned h);

		/*!
		 * Copy another Bitmap to the current Bitmap at position (x,y).
		 */
		virtual void blit(const Bitmap &src, int x, int y);

		virtual void clear(bool enable);

		virtual unsigned getWidth() const { return m_width; }
		virtual unsigned getHeight() const { return m_height; }

		virtual void debugPrint(void) const;
};

#endif // BITMAP_H

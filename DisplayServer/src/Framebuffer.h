#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <string>

#include "Bitmap.h"

class Framebuffer : public Bitmap
{
	public:
		struct TextArea {
			unsigned x, y, w, h;

			TextArea()
			{
				clear();
			}

			void clear(void)
			{
				w = h = 0;
			}
		};

	private:
		struct TextArea m_textArea;

	public:
		static const unsigned WIDTH = 160;
		static const unsigned HEIGHT = 24;

		Framebuffer();
		~Framebuffer();

		void serialize(std::string *buffer) const;

		void setTextArea(unsigned x, unsigned y, unsigned w, unsigned h);
};

#endif // FRAMEBUFFER_H

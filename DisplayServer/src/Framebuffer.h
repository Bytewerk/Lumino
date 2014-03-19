#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <string>

#include "Bitmap.h"

class Framebuffer : public Bitmap
{
	public:
		static const unsigned WIDTH = 160;
		static const unsigned HEIGHT = 24;

		Framebuffer();
		~Framebuffer();

		void serialize(std::string *buffer);
};

#endif // FRAMEBUFFER_H

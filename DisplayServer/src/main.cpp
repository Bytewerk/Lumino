#include <ft2build.h>
#include FT_FREETYPE_H

#include <math.h>

#include <iostream>
#include <string>
#include <sstream>

#include <ctime>

#include "Font.h"
#include "Logger.h"
#include "Bitmap.h"
#include "Framebuffer.h"
#include "Exception.h"

#include "TCPServer.h"
#include "TCPSocket.h"

#include "strutil.h"

using namespace std;

void demo(FT_Library *ftlib) {
	Framebuffer fb;
	string serialData;
	Font demoFont(ftlib, "/usr/share/fonts/dejavu/DejaVuSans.ttf", 10);

	unsigned frameIndex = 0;

	while(true) {
		fb.clear(false);

		for(unsigned x = 0; x < Framebuffer::WIDTH/16; x++) {
			for(unsigned y = 0; y < Framebuffer::HEIGHT; y++) {
				bool even = ((y % 2) == 0);
				unsigned mx;

				if(even) {
					mx = (16*x + frameIndex) % Framebuffer::WIDTH;
				} else {
					// the added (16 << 24) is necessary, as C(++) doesn’t support modulo
					// on negative numbers
					mx = (16*x + (16 << 24) - frameIndex) % Framebuffer::WIDTH;
				}

				fb.setPixel(mx, y, true);
			}
		}

		wostringstream oss;
		oss << L"?¿? D€MØ " << frameIndex << L" D€MØ !¡!";

		Bitmap textBitmap(0, 0);
		textBitmap.clear(false);
		demoFont.renderText(oss.str(), &textBitmap);

		unsigned x = 80 - textBitmap.getWidth()/2  + 20 * cos(2 * M_PI * frameIndex / 200);
		unsigned y = 12 - textBitmap.getHeight()/2 + 9 * sin(2 * M_PI * frameIndex / 191);
		fb.blit(textBitmap, x, y);

		fb.serialize(&serialData);
		cout << serialData << flush;

		frameIndex++;

		//struct timespec ts = {0, 1000000000/60};
		//nanosleep(&ts, NULL);
	}
}

int main(void)
{
	FT_Library ftlib;
	Framebuffer fb;
	int fterror = 0;

	std::setlocale(LC_ALL, "en_US.UTF-8");

	LOG(Logger::LVL_INFO, "main", "This is DisplayServer v" VERSION);

	fterror = FT_Init_FreeType(&ftlib);
	if(fterror) {
		LOG(Logger::LVL_FATAL, "main", "Failed to initialize freetype: %i", fterror);
		return 1;
	}

	Font testFont(&ftlib, "/usr/share/fonts/dejavu/DejaVuSans.ttf", 10);

	std::wstring testtext = mb_to_wstring(u8"Hello FreeType! With Ünïĉødè: ∫x²dx=x³/₃");

	Bitmap textBitmap(0, 0);
	testFont.renderText(testtext, &textBitmap);

	fb.blit(textBitmap, 0, 0);

	std::string serialData;
	fb.serialize(&serialData);
	cout << serialData;

	try {
		TCPServer server(12345);
		server.start();

		while(true) {
			TCPSocket socket = server.acceptConnection();

			socket.send("Please enter your name: ");
			socket.send("Hello, " + socket.recv());
		}
	} catch(Exception &e) {
		LOG(Logger::LVL_FATAL, "main", "Exception [%s]: %s", e.module().c_str(), e.message().c_str());
		return 1;
	}

	demo(&ftlib);
}

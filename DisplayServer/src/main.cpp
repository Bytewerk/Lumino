#include <ft2build.h>
#include FT_FREETYPE_H

#include <math.h>
#include <signal.h>

#include <iostream>
#include <string>
#include <sstream>
#include <map>

#include <ctime>

#include "Font.h"
#include "Base64.h"
#include "Logger.h"
#include "Bitmap.h"
#include "Framebuffer.h"
#include "Exception.h"

#include "TCPServer.h"
#include "TCPSocket.h"
#include "SocketSelector.h"

#include "strutil.h"
#include "timeutils.h"

using namespace std;

typedef map<int, string> ClientDataMap;

const double FRAME_INTERVAL = 1.0 / 24;

Framebuffer *fb;
FT_Library ftlib;

void commit_screen(void) {
	std::string serialData;
	fb->serialize(&serialData);
	cout << serialData << flush;
}

void demo(unsigned nframes) {
	string serialData;
	Font demoFont(&ftlib, "/usr/share/fonts/dejavu/DejaVuSans.ttf", 10);

	unsigned frameIndex = 0;

	while(frameIndex < nframes) {
		fb->clear(false);

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

				fb->setPixel(mx, y, true);
			}
		}

		wostringstream oss;
		oss << L"?¿? D€MØ " << frameIndex << L" D€MØ !¡!";

		Bitmap textBitmap(0, 0);
		textBitmap.clear(false);
		demoFont.renderText(oss.str(), &textBitmap);

		unsigned x = 80 - textBitmap.getWidth()/2  + 20 * cos(2 * M_PI * frameIndex / 200);
		unsigned y = 12 - textBitmap.getHeight()/2 + 9 * sin(2 * M_PI * frameIndex / 191);
		fb->blit(textBitmap, x, y);

		commit_screen();

		frameIndex++;

		//struct timespec ts = {0, 1000000000/24};
		//nanosleep(&ts, NULL);
	}
}

bool process_command(const string &line)
{
	static Base64 b64(Base64::DEFAULT_MAPPING);

	vector<string> parts;
	split(line, &parts);

	// capture IOExceptions
	try {
		if(parts[0] == "demo") {
			// demo takes 1 parameter: number of frames
			if(parts.size() != 2) {
				return false;
			}

			unsigned nframes = to_var<unsigned>(parts[1]);

			demo(nframes);
			return true;
		} else if(parts[0] == "setpixel") {
			// setpixel takes 3 parameters: x, y, enable
			if(parts.size() != 4) {
				return false;
			}

			unsigned x = to_var<unsigned>(parts[1]);
			unsigned y = to_var<unsigned>(parts[2]);
			bool enable = to_var<bool>(parts[3]);

			fb->setPixel(x, y, enable);

			return true;
		} else if(parts[0] == "commit") {
			commit_screen();
			return true;
		} else if(parts[0] == "clear") {
			// clear takes 1 parameter: enable
			if(parts.size() != 2) {
				return false;
			}

			bool enable = to_var<bool>(parts[1]);

			fb->clear(enable);
			return true;
		} else if(parts[0] == "drawbitmap") {
			// drawbitmap takes 5 parameters: x y w h base64-data
			if(parts.size() != 6) {
				return false;
			}

			unsigned x = to_var<unsigned>(parts[1]);
			unsigned y = to_var<unsigned>(parts[2]);
			unsigned w = to_var<unsigned>(parts[3]);
			unsigned h = to_var<unsigned>(parts[4]);
			string data = b64.decode(parts[5]);

			Bitmap bmp(w, h);
			bmp.setData(data);

			fb->blit(bmp, x, y);
			return true;
		} else if(parts[0] == "setfb") {
			// setfb takes 1 parameter: data
			if(parts.size() != 2) {
				return false;
			}

			string data = b64.decode(parts[1]);

			fb->setData(data);
			return true;
		} else if(parts[0] == "drawtext") {
			// drawtext takes 3 parameter: x, y, text
			if(parts.size() < 4) {
				return false;
			}

			unsigned x = to_var<unsigned>(parts[1]);
			unsigned y = to_var<unsigned>(parts[2]);

			ostringstream utf8text;
			for(unsigned i = 3; i < parts.size(); i++) {
				utf8text << parts[i] << " ";
			}

			wstring text = mb_to_wstring(utf8text.str());

			fb->drawText(x, y, text);

			return true;
		} else if(parts[0] == "settext") {
			// settext takes 2 parameters: line, text
			if(parts.size() < 3) {
				return false;
			}

			unsigned line = to_var<unsigned>(parts[1]);

			ostringstream utf8text;
			for(unsigned i = 2; i < parts.size(); i++) {
				utf8text << parts[i] << " ";
			}

			wstring text = mb_to_wstring(utf8text.str());

			fb->setText(line, text);

			return true;
		} else if(parts[0] == "settextarea") {
			// settext takes 4 parameters: x, y, w, h
			if(parts.size() != 5) {
				return false;
			}

			unsigned x = to_var<unsigned>(parts[1]);
			unsigned y = to_var<unsigned>(parts[2]);
			unsigned w = to_var<unsigned>(parts[3]);
			unsigned h = to_var<unsigned>(parts[4]);

			fb->setTextArea(x, y, w, h);

			return true;
		}
	} catch(IOException &e) {
		LOG(Logger::LVL_ERR, "process_command", "IOException caught [%s]: %s", e.module().c_str(), e.message().c_str());
		return false;
	} catch(Exception &e) {
		LOG(Logger::LVL_ERR, "process_command", "Exception caught [%s]: %s", e.module().c_str(), e.message().c_str());
		return false;
	}

	// command not recognized
	return false;
}

int main(void)
{
	ClientDataMap clientData;
	int fterror = 0;

	double nextFrameTime = get_hires_time() + FRAME_INTERVAL;

	std::setlocale(LC_ALL, "en_US.UTF-8");

	LOG(Logger::LVL_INFO, "main", "This is DisplayServer v" VERSION);

	// ignore SIGPIPE to prevent program shutdown on disconnected sockets
	struct sigaction sa;
	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);

	// initialize the freetype library
	fterror = FT_Init_FreeType(&ftlib);
	if(fterror) {
		LOG(Logger::LVL_FATAL, "main", "Failed to initialize freetype: %i", fterror);
		return 1;
	}

	fb = new Framebuffer(&ftlib);

	// start the server
	try {
		TCPServer server(12345);
		SocketSelector selector;

		server.start();

		while(true) {
			if(server.hasPendingConnection()) {
				TCPSocket socket = server.acceptConnection();
				socket.send("Welcome to DisplayServer v" VERSION "\n");
				selector.addSocket(socket);

				LOG(Logger::LVL_DEBUG, "main", "Client %i connected.", socket.getFileDescriptor());

				clientData[socket.getFileDescriptor()] = "";
			}

			selector.select();

			for(TCPSocket socket : selector.getReadSockets()) {
				try {
					string inData = socket.recv();

					if(inData.empty()) {
						// empty recv-data => socket closed on remote side
						LOG(Logger::LVL_DEBUG, "main", "Client %i disconnected.", socket.getFileDescriptor());
						socket.close();
						selector.removeSocket(socket);
					}

					int fd = socket.getFileDescriptor();
					clientData[fd].append(inData);

					// check if a complete line was received
					string line;
					while(get_line_from_string(&(clientData[fd]), &line)) {
						if(process_command(line)) {
							socket.send("200 OK\n");
						} else {
							socket.send("405 Invalid Request\n");
						}
					}
				} catch(NetworkingException &e) {
					LOG(Logger::LVL_ERR, "main", "Exception caught [%s]: %s", e.module().c_str(), e.message().c_str());
					socket.close();
					selector.removeSocket(socket);
				}
			}

			// redraw and scroll text
			fb->shiftText();
			fb->redrawText();

			commit_screen();

			// FPS limiter
			sleep_until(nextFrameTime);
			nextFrameTime += FRAME_INTERVAL;
		}
	} catch(Exception &e) {
		LOG(Logger::LVL_FATAL, "main", "Exception [%s]: %s", e.module().c_str(), e.message().c_str());
		return 1;
	}

	delete fb;
}

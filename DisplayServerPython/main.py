#!/usr/bin/env python2

from __future__ import print_function

import codecs
import time
import sys
import math
import base64

import threading

import SocketServer

from framebuffer import *
from bitmap import *
from font import *

fb = FrameBuffer()
fbsema = threading.Semaphore(1)

class ScrollingThread(threading.Thread):
	def run(self):
		while True:
			fbsema.acquire()
			fb.shiftText()
			fb.redrawText()
			commit_screen()
			fbsema.release()

			time.sleep(0.10)


def commit_screen():
	print(fb.serialize())
	sys.stdout.flush()

def run_demo(cmdparts):
	t = 0
	while t < 240:
		# update the framebuffer
		fb.clear()

		for x in range(DISPLAY_WIDTH / 16):
			for y in range(DISPLAY_HEIGHT):
				even = (y % 2) == 0
				if even:
					mx = (16*x + t) % DISPLAY_WIDTH
				else:
					mx = (16*x - t) % DISPLAY_WIDTH
				fb.setPixel(mx, y, True)

		for x in range(DISPLAY_WIDTH):
			fb.setPixel(x, int(11.5 * (1+math.sin(2*math.pi * 10 * float(t)/DISPLAY_WIDTH) * math.sin(2*math.pi * 2 * (float(x)/DISPLAY_WIDTH)))), True)

		# send the framebuffer
		commit_screen()

		time.sleep(0.01)

		t += 1

	return True

def run_setpixel(cmdparts):
	# setpixel needs 3 params: x, y and enable
	if len(cmdparts) != 4:
		return False

	x = int(cmdparts[1])
	y = int(cmdparts[2])
	enable = int(cmdparts[3]) != 0

	return fb.setPixel(x, y, enable)

def run_clear(cmdparts):
	# setpixel needs 0 or 1 params:
	# no param: clear to black
	#  1 param: clear to param (0 is black, 1 is white)
	if len(cmdparts) == 1:
		clr = 0
	elif len(cmdparts) == 2:
		clr = int(cmdparts[1]) != 0
	else:
		return False

	fb.clear(clr)

	return True

def run_commit(cmdparts):
	commit_screen()
	return True

def run_setfb(cmdparts):
	# setpixel needs 1 param: Base64 encoded framebuffer data
	if len(cmdparts) != 2:
		return False

	s = base64.b64decode(cmdparts[1]);

	try:
		fb.updateFromString(s)
		return True
	except Exception:
		return False

def run_drawbitmap(cmdparts):
	# drawbitmap needs 5 params: x, y, width, height, data (Base64-encoded)
	if len(cmdparts) != 6:
		return False

	x = int(cmdparts[1])
	y = int(cmdparts[2])
	w = int(cmdparts[3])
	h = int(cmdparts[4])

	# 8x7-bitmap example data
	#data = "\x18\x24\x42\xA5\x81\x66\x3C\x18"
	data = base64.b64decode(cmdparts[5]);

	bmp = Bitmap(w, h, data)
	fb.drawBitmap(x, y, bmp)
	return True

def run_drawtext(cmdparts):
	# drawtext needs 3 params: x, y, text
	if len(cmdparts) < 4:
		return False

	x = int(cmdparts[1])
	y = int(cmdparts[2])

	s = " ".join(cmdparts[3:])

	bmp = font.getBitmap(s)
	fb.drawBitmap(x, y, bmp)
	return True

def run_settext(cmdparts):
	# settext needs 2 params: line, text
	if len(cmdparts) < 3:
		return False

	line = int(cmdparts[1])

	s = " ".join(cmdparts[2:])

	fb.setText(line, s)
	return True

def run_settextarea(cmdparts):
	# settext needs 4 params: x, y, w, h
	if len(cmdparts) != 5:
		return False

	x = int(cmdparts[1])
	y = int(cmdparts[2])
	w = int(cmdparts[3])
	h = int(cmdparts[4])

	fb.setTextArea(x, y, w, h)
	return True

def run_cleartextarea(cmdparts):
	# settext needs no params
	if len(cmdparts) != 1:
		return False

	fb.clearTextArea()
	return True

class MyTCPHandler(SocketServer.StreamRequestHandler):
	def handle(self):
		# receive the data from the client
		for cmd in self.rfile:
			cmd = cmd.strip().decode('utf8')
			cmdparts = cmd.split(u' ')

			#print("Received: [%d, %s] %s" % (len(cmdparts), type(cmd), cmd), file=sys.stderr)

			#try:
			if True:
				cmdHandler = None

				if cmdparts[0] == "demo":
					cmdHandler = run_demo
				elif cmdparts[0] == "setpixel":
					cmdHandler = run_setpixel
				elif cmdparts[0] == "commit":
					cmdHandler = run_commit
				elif cmdparts[0] == "clear":
					cmdHandler = run_clear
				elif cmdparts[0] == "setfb":
					cmdHandler = run_setfb
				elif cmdparts[0] == "drawbitmap":
					cmdHandler = run_drawbitmap
				elif cmdparts[0] == "drawtext":
					cmdHandler = run_drawtext
				elif cmdparts[0] == "settext":
					cmdHandler = run_settext
				elif cmdparts[0] == "settextarea":
					cmdHandler = run_settextarea
				elif cmdparts[0] == "cleartextarea":
					cmdHandler = run_cleartextarea

				if cmdHandler:
					fbsema.acquire()
					result = cmdHandler(cmdparts)
					fbsema.release()

					if result:
						self.request.sendall("200 OK\n")
					else:
						self.request.sendall("405 Invalid request\n")
				else:
					self.request.sendall("400 Unknown command\n")
			#except Exception:
			#	self.request.sendall("500 Fail\n")

if __name__ == "__main__":
	HOST, PORT = "", 12345

	fb.clear(0)
	#fb.setTextArea(20, 1, 5*32-20, 21)
	#fb.setText(0, "Hello")
	#fb.setText(1, "bytewerk!!!!11!1!!!einundelfzig11111!!1!")
	#run_demo([''])

	st = ScrollingThread()
	st.daemon = True
	st.start()

	# Create the server
	server = SocketServer.TCPServer((HOST, PORT), MyTCPHandler)

	# Activate the server; this will keep running until you
	# interrupt the program with Ctrl-C
	server.serve_forever()

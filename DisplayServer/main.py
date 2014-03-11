#!/usr/bin/env python2

import codecs
import time
import sys

from framebuffer import *

f = FrameBuffer()

offset = 0

while True:
	# update the framebuffer
	f.clear()

	for x in range(DISPLAY_WIDTH / 16):
		for y in range(DISPLAY_HEIGHT):
			even = (y % 2) == 0
			if even:
				mx = (16*x + offset) % DISPLAY_WIDTH
			else:
				mx = (16*x - offset) % DISPLAY_WIDTH
			f.setPixel(mx, y, True)

	# send the framebuffer
	print(f.serialize())
	sys.stdout.flush()

	time.sleep(0.05)

	offset += 1

#!/usr/bin/env python2

import codecs
import time
import sys
import math

from framebuffer import *

f = FrameBuffer()

t = 0

while True:
	# update the framebuffer
	f.clear()

	for x in range(DISPLAY_WIDTH / 16):
		for y in range(DISPLAY_HEIGHT):
			even = (y % 2) == 0
			if even:
				mx = (16*x + t) % DISPLAY_WIDTH
			else:
				mx = (16*x - t) % DISPLAY_WIDTH
			f.setPixel(mx, y, True)

	for x in range(DISPLAY_WIDTH):
		f.setPixel(x, int(11.5 * (1+math.sin(2*math.pi * 10 * float(t)/DISPLAY_WIDTH) * math.sin(2*math.pi * 2 * (float(x)/DISPLAY_WIDTH)))), True)

	# send the framebuffer
	print(f.serialize())
	sys.stdout.flush()

	time.sleep(1.0/24)

	t += 1

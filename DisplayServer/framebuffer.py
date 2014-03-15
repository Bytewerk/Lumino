from font import Font
from bitmap import Bitmap

DISPLAY_WIDTH = 160
DISPLAY_HEIGHT = 24

DATA_SIZE = DISPLAY_WIDTH*DISPLAY_HEIGHT/8

OVERSCROLL = 10

class TextArea:
	x = 0
	y = 0
	w = 0
	h = 0

	def __init__(self):
		self.clear()

	def set(self, x, y, w, h):
		self.x, self.y, self.w, self.h = x, y, w, h

	def clear(self):
		self.w = 0
		self.h = 0

	def isClear(self):
		return (self.w == 0 and self.h == 0)

	def isInside(self, x, y):
		return (x >= self.x and y >= self.y and x < (self.x + self.w) and y < (self.y + self.h))

class FrameBuffer:
	_textarea = TextArea()

	def __init__(self):
		self._data = [0 for i in range(DATA_SIZE)]
		self.clearTextArea()
		self._textBmps = [None, None]
		self._texts = [u"", u""]
		self._textOffsets = [0, 0]
		self._font = Font(9)

	def setPixel(self, x, y, on):
		# range check
		if x < 0 or y < 0 or x >= DISPLAY_WIDTH or y >= DISPLAY_HEIGHT:
			return False

		bit  = (y * DISPLAY_WIDTH + x)
		byte = bit / 8
		bit %= 8

		if on:
			self._data[byte] |= (0x80 >> bit);
		else:
			self._data[byte] &= ~(0x80 >> bit);

		return True

	def setText(self, line, s):
		if line > 1:
			raise IndexError()

		if self._texts[line] != s:
			self._textBmps[line] = self._font.getBitmap(s)
			self._texts[line] = s

	def redrawText(self):
		# do not render anything if textarea is clear
		if self._textarea.isClear():
			return

		for line in range(2):
			bmp = self._textBmps[line]

			if not bmp:
				continue

			if self._textOffsets[line] >= 0:
				sx = self._textarea.x
			elif self._textOffsets[line] < (self._textarea.w - self._textBmps[line].width):
				sx = self._textarea.x + self._textarea.w - self._textBmps[line].width
			else:
				sx = self._textarea.x + self._textOffsets[line]

			sy = self._textarea.y + bmp.height * line

			for dx in range(bmp.width):
				for dy in range(bmp.height):
					x = sx+dx
					y = sy+dy

					if self._textarea.isInside(x, y):
						self.setPixel(x, y, bmp.isOn(dx, dy))

	def shiftText(self):
		for line in range(2):
			if not self._textBmps[line]:
				continue

			# check if scrolling is necessary
			if self._textBmps[line].width > self._textarea.w:
				self._textOffsets[line] -= 1

				# if text would vanish on the left, reset it
				if self._textOffsets[line] < (self._textarea.w - self._textBmps[line].width - OVERSCROLL):
					self._textOffsets[line] = OVERSCROLL

	def clear(self, on = False):
		for i in range(DATA_SIZE):
			if on:
				self._data[i] = 0xFF
			else:
				self._data[i] = 0x00

	def updateFromString(self, s):
		if len(s) >= DATA_SIZE:
			self._data = [ord(s[i]) for i in range(DATA_SIZE)]
		else:
			raise IndexError()

	def drawBitmap(self, x, y, bitmap):
		for dx in range(bitmap.width):
			for dy in range(bitmap.height):
				self.setPixel(x+dx, y+dy, bitmap.isOn(dx, dy))

	def setTextArea(self, x, y, w, h):
		self._textarea.set(x, y, w, h)

	def clearTextArea(self):
		self._textarea.clear()

	def serialize(self):
		s = "\x1b\x02"
		for byte in self._data:
			if byte == 0x1B:
				# this is the escape byte, which must be repeated in data
				s += chr(byte)
			s += chr(byte)

		s += "\x1b\x03"
		return s

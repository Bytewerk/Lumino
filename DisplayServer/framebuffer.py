DISPLAY_WIDTH = 160
DISPLAY_HEIGHT = 24

DATA_SIZE = DISPLAY_WIDTH*DISPLAY_HEIGHT/8

class FrameBuffer:
	def __init__(self):
		self._data = [0 for i in range(DATA_SIZE)]

	def setPixel(self, x, y, on):
		bit  = (y * DISPLAY_WIDTH + x)
		byte = bit / 8
		bit %= 8

		if on:
			self._data[byte] |= (0x80 >> bit);
		else:
			self._data[byte] &= ~(0x80 >> bit);

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

	def serialize(self):
		s = "\x1b\x02"
		for byte in self._data:
			if byte == 0x1B:
				# this is the escape byte, which must be repeated in data
				s += chr(byte)
			s += chr(byte)

		s += "\x1b\x03"
		return s


class Bitmap:
	width = 0
	height = 0
	_data = []

	def __init__(self, w, h, data):
		datalen = w*h / 8 + 1

		self.width = w
		self.height = h

		if len(data) < datalen:
			raise IndexError()

		self._data = [ord(data[i]) for i in range(datalen)]

	
	def isOn(self, x, y):
		if x < 0 or y < 0 or x >= self.width or y >= self.height:
			return 0

		bit  = (y * self.width + x)
		byte = bit / 8
		bit %= 8

		return (self._data[byte] & (0x80 >> bit)) != 0

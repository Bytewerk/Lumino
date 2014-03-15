from PIL import Image, ImageDraw, ImageFont

from bitmap import Bitmap

class Font:
	def __init__(self, fontsize):
		self._fontsize = fontsize
		self._font = ImageFont.truetype("/usr/share/fonts/dejavu/DejaVuSans.ttf", fontsize)

	# create a Bitmap object from a string
	def getBitmap(self, s):
		w = len(s)*self._fontsize
		h = int(round(self._fontsize * 1.2))

		size = (w, h)
		image = Image.new("1", size, 0)
		draw = ImageDraw.Draw(image)
		draw = draw.text((0,0), s, 1, font=self._font)

		# determine the actually needed width for this bitmap
		maxw = 0
		for x in range(w):
			for y in range(h):
				if image.getpixel( (x, y) ) and x > maxw:
					maxw = x

		maxw += 2

		tmpdata = [0 for i in range(maxw*h+1)]
		for x in range(maxw):
			for y in range(h):
				bit  = (y * maxw + x)
				byte = bit / 8
				bit %= 8

				if image.getpixel( (x, y) ):
					tmpdata[byte] |= (0x80 >> bit)

		data = [chr(c) for c in tmpdata]

		return Bitmap(maxw, h, data)

from PIL import Image, ImageDraw, ImageFont

from bitmap import Bitmap

image = Image.new("1", (100,8), 1)
usr_font = ImageFont.truetype("/usr/share/fonts/dejavu/DejaVuSansMono.ttf", 8)
d_usr = ImageDraw.Draw(image)
d_usr = d_usr.text((0,0), "Travis L.",0, font=usr_font)

image.save("test.png")

class Font:
	def __init__(self, fontsize):
		self._fontsize = fontsize
		self._font = ImageFont.truetype("/usr/share/fonts/dejavu/DejaVuSansMono.ttf", fontsize)

	# create a Bitmap object from a string
	def getBitmap(self, s):
		w = len(s)*self._fontsize
		h = self._fontsize

		size = (w, h)
		image = Image.new("1", size, 0)
		draw = ImageDraw.Draw(image)
		draw = draw.text((0,0), s, 1, font=self._font)

		tmpdata = [0 for i in range(w*h+1)]
		for x in range(w):
			for y in range(h):
				bit  = (y * w + x)
				byte = bit / 8
				bit %= 8

				if image.getpixel( (x, y) ):
					tmpdata[byte] |= (0x80 >> bit)

		data = [chr(c) for c in tmpdata]

		return Bitmap(w, h, data)

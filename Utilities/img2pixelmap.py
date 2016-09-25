#!/usr/bin/env python3
import argparse
from argparse import RawTextHelpFormatter
from PIL import Image
import base64

parser = argparse.ArgumentParser(description="A little converter that processes images to be sent to the bytewerk lumino displayserver.", formatter_class=RawTextHelpFormatter)
parser.add_argument("inputFile", help="PIL compatible input image (BMP, PNG, JPEG, …)")
parser.add_argument("--outputFormat", choices=["0", "1", "2"], default=0, help=
		"Defines the output format\n"
		"0 = default, drawbitmap compatible base64 string\n"
		"1 = text representation\n"
		"2 = pastable bash command line with echo and socat"
		)
parser.add_argument("--x", default=0, help="Sets the x-position if the output format is a pastable bash command (2)")
parser.add_argument("--y", default=0, help="Same as --x but for the y-axis")

args = parser.parse_args()

# Try to open the supplied file
try:
	img = Image.open(args.inputFile)
except OSError:
	print(args.inputFile + " is not a PIL compatible image. Exiting.")
	exit(1)

# RGB convertion is needed for pixel colour reading
rgb_img = img.convert("RGB")
bit_string = ""

# Read pixel from top left to bottom right
for line in range(0, img.height):
	for row in range(0, img.width):
		r, g, b = rgb_img.getpixel((row, line))
		if r is 255 and g is 255 and b is 255:
			bit_string += "0" # White pixel result in a 0
		else:
			bit_string += "1" # Every other colour results in a 1

# Generates a text representation containing 0 and 1 with the linebreak at the right place
def generateText():
	for i in range(0, len(bit_string)):
		# Check if we reached the end of the picture and print a linebreak
		if i % img.width == 0 and i > 0:
			print("\n", end="")
		# Print the actual "pixel"
		print(bit_string[i], end="")
	# A final linebreak for pretty bash output
	print("\n", end="")

def generateBase64String(bit_string):
	# Take an empty array
	byte_array = []
	while len(bit_string) >= 8:
		# Put the last 8 bits of the bit_string (a byte) infront of the byte_array
		byte_array = [int(bit_string[-8:], 2)] + byte_array
		# And then remove those last 8 bits
		bit_string = bit_string[:-8]
	# Check if bits are left over and create a last filled byte
	if len(bit_string) > 0 and len(bit_string) < 8:
		filled_string = "0" * (8 - len(bit_string)) + bit_string
		byte_array = [int(filled_string, 2)] + byte_array

	# This is a workaround for a bug in the display server. If the total image size is dividable by 8 it needs another byte.
	if img.width * img.height / 8 == 0:
		byte_array.append(255)
	
	# Cast the fake byte_array into an actual bytearray and encode it to receive the desired string
	b64 = base64.b64encode(bytearray(byte_array))
	return b64.decode("UTF-8")


if args.outputFormat == "0":
	# Base64 representation was choosen
	print(generateBase64String(bit_string))
elif args.outputFormat == "1":
	# Text representation was choosen
	generateText()
elif args.outputFormat == "2":
	# Ready to paste bash command was choosen
	print("echo \"drawbitmap 0 0 {1} {2} {0}\" | socat STDIO tcp:ledschild.bingo:12345".format(generateBase64String(bit_string), img.width, img.height))

exit(0)


# vim:ts=2:noexpandtab:sts=2:sw=2

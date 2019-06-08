#!/usr/bin/env python3

import mpd
import time
import socket
import base64

progressbuffer = base64.b64encode(bytes([255]*40)).decode('ascii')

# 16x15 icon
playsymbol = base64.b64encode(bytes.fromhex("08000C000E000F000F800FC00FE00FF00FE00FC00F800F000E000C00080000")).decode('ascii')
pausesymbol = base64.b64encode(bytes.fromhex("000000003C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C0000000000")).decode('ascii')
stopsymbol = base64.b64encode(bytes.fromhex("000000003FFC3FFC3FFC3FFC3FFC3FFC3FFC3FFC3FFC3FFC3FFC0000000000")).decode('ascii')

while True:
	try:
		mpdclient = mpd.MPDClient()
		mpdclient.connect('localhost', 6600)

		displaysocket = socket.create_connection(('ledschild.bytewerk.in', 12345))
		displaysocket.sendall("setprio 1000000000\n".encode('utf-8'))
		displaysocket.sendall("settextarea 16 0 144 22\n".encode('utf-8'))

		while True:
			status = mpdclient.status()

			if 'song' in status.keys():
				cursong = int(status['song'])

				playlist = mpdclient.playlistinfo()
				cursonginfo = playlist[cursong]

				if 'title' not in cursonginfo.keys():
					title = "KeyError: 'title'"
				else:
					title = cursonginfo['title']

				if 'artist' not in cursonginfo.keys():
					artist = "KeyError: 'artist'"
				else:
					artist = cursonginfo['artist']

				cmd = "settext 0 %s\n" % artist
				displaysocket.sendall(cmd.encode('utf-8'))

				cmd = "settext 1 %s\n" % title
				displaysocket.sendall(cmd.encode('utf-8'))

				# progress
				if 'time' in status.keys():
					parts = status['time'].split(':')
					playedtime = float(parts[0])
					fulltime = float(parts[1])

					displaysocket.sendall("clear\n".encode('utf-8'))
					if fulltime != 0:
						width = int(160 * playedtime / fulltime)

						displaysocket.sendall(("drawbitmap 0 22 %d 2 %s\n" % (width, progressbuffer)).encode('utf-8'))

			if status['state'] == "stop":
				displaysocket.sendall(("drawbitmap 0 2 16 15 %s\n" % stopsymbol).encode('utf-8'))
			elif status['state'] == "pause":
				displaysocket.sendall(("drawbitmap 0 2 16 15 %s\n" % pausesymbol).encode('utf-8'))
			elif status['state'] == "play":
				displaysocket.sendall(("drawbitmap 0 2 16 15 %s\n" % playsymbol).encode('utf-8'))

			else:
				displaysocket.sendall(("settext 0 Let the music play!\n").encode('utf-8'))
				displaysocket.sendall(("settext 1 ...\n").encode('utf-8'))

			time.sleep(1)
	except Exception as e:
                print(str(e))

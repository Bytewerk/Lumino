#!/usr/bin/env python2

import mpd
import time
import socket
import base64

progressbuffer = base64.b64encode(''.join([chr(255) for i in range(40)]))

# 16x15 icon
playsymbol = base64.b64encode("08000C000E000F000F800FC00FE00FF00FE00FC00F800F000E000C00080000".decode('hex'))
pausesymbol = base64.b64encode("000000003C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C3C0000000000".decode('hex'))
stopsymbol = base64.b64encode("000000003FFC3FFC3FFC3FFC3FFC3FFC3FFC3FFC3FFC3FFC3FFC0000000000".decode('hex'))

while True:
	try:
		mpdclient = mpd.MPDClient()
		mpdclient.connect('music', 6600)

		displaysocket = socket.create_connection(('ledschild', 12345))
		displaysocket.sendall("settextarea 16 0 144 22\n")

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
				displaysocket.sendall(cmd)

				cmd = "settext 1 %s\n" % title
				displaysocket.sendall(cmd)

				# progress
				if 'time' in status.keys():
					parts = status['time'].split(':')
					playedtime = float(parts[0])
					fulltime = float(parts[1])

					width = int(160 * playedtime / fulltime)

					displaysocket.sendall("clear\n")
					displaysocket.sendall("drawbitmap 0 22 %d 2 %s\n" % (width, progressbuffer))

			if status['state'] == "stop":
				displaysocket.sendall("drawbitmap 0 2 16 15 %s\n" % stopsymbol)
			elif status['state'] == "pause":
				displaysocket.sendall("drawbitmap 0 2 16 15 %s\n" % pausesymbol)
			elif status['state'] == "play":
				displaysocket.sendall("drawbitmap 0 2 16 15 %s\n" % playsymbol)

			else:
				displaysocket.sendall("settext 0 Let the music play!\n")
				displaysocket.sendall("settext 1 ...\n")

			time.sleep(1)
	except Exception:
		pass

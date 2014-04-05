#ifndef SOCKETSELECTOR_H
#define SOCKETSELECTOR_H

#include <map>
#include <vector>

#include "TCPSocket.h"

class SocketSelector {
	public:
		typedef std::map<int, TCPSocket> SocketMap;
		typedef std::vector<TCPSocket> SocketList;

	private:
		SocketMap m_sockets;
		int       m_maxFD;

		SocketList m_readSockets, m_writeSockets;

		void updateMaxFD(void);

	public:
		SocketSelector();

		void addSocket(const TCPSocket &s);
		void removeSocket(const TCPSocket &s);

		void select();

		const SocketList& getReadSockets(void) const { return m_readSockets; }
		const SocketList& getWriteSockets(void) const { return m_writeSockets; }
};

#endif // SOCKETSELECTOR_H

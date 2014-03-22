#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Exception.h"

#include "SocketSelector.h"

SocketSelector::SocketSelector()
	: m_maxFD(0)
{
}

void SocketSelector::addSocket(const TCPSocket &s)
{
	int fd = s.getFileDescriptor();

	m_sockets[fd] = s;

	if(fd > m_maxFD) {
		m_maxFD = fd;
	}
}

void SocketSelector::removeSocket(const TCPSocket &s)
{
	int fd = s.getFileDescriptor();

	m_sockets.erase(fd);
}

void SocketSelector::select()
{
	fd_set rfds, wfds;
	struct timeval tv = {0, 0};

	// fill the sets
	for(auto &kv: m_sockets) {
		FD_SET(kv.first, &rfds);
		FD_SET(kv.first, &wfds);
	}

	// run the select
	int result = ::select(m_maxFD + 1, &rfds, &wfds, NULL, &tv);

	if(result < 0) {
		throw NetworkingException("SocketSelector", "select() failed.");
	}

	// fill the result vectors from the resulting sets
	m_readSockets.clear();
	m_writeSockets.clear();

	for(auto &kv: m_sockets) {
		if(FD_ISSET(kv.first, &rfds)) {
			m_readSockets.push_back(kv.second);
		}

		if(FD_ISSET(kv.first, &wfds)) {
			m_writeSockets.push_back(kv.second);
		}
	}
}

#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Exception.h"
#include "Logger.h"

#include "SocketSelector.h"

void SocketSelector::updateMaxFD(void)
{
	// do nothing if there are no sockets available
	if(m_sockets.empty()) {
		return;
	}

	SocketMap::const_iterator iter = m_sockets.end();
	iter--;
	m_maxFD = iter->first;
}

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

	updateMaxFD();
}

void SocketSelector::removeSocket(const TCPSocket &s)
{
	int fd = s.getFileDescriptor();

	size_t nErased = m_sockets.erase(fd);

	LOG(Logger::LVL_DEBUG, "SocketSelector", "Erased %u elements.", nErased);

	updateMaxFD();
}

void SocketSelector::select()
{
	fd_set rfds, wfds;
	struct timeval tv = {0, 0};

	// if there are no sockets in list, just return nothing
	if(m_sockets.empty()) {
		m_readSockets.clear();
		m_writeSockets.clear();
		return;
	}

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

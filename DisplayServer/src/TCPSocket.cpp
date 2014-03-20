#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "Exception.h"

#include "TCPSocket.h"

TCPSocket::TCPSocket(void)
	: m_socket(0)
{
}

TCPSocket::TCPSocket(int socket)
	: m_socket(socket)
{
}

TCPSocket::TCPSocket(const std::string &addr, uint16_t port)
{
	throw Exception("TCPSocket", "Connecting is not implemented yet!");
}

TCPSocket::TCPSocket(const TCPSocket &s)
	: m_socket(s.m_socket)
{
}

TCPSocket::~TCPSocket()
{
}

void TCPSocket::send(const std::string &data) const
{
	ssize_t bytesSent;
	ssize_t totalBytes = 0;

	do {
		bytesSent = ::send(m_socket, data.data() + totalBytes, data.size() - totalBytes, 0);

		if(bytesSent < 0) {
			throw NetworkingException("TCPSocket", "send() failed.");
		}

		totalBytes += bytesSent;
	} while(totalBytes < data.length());
}

std::string TCPSocket::recv(void) const
{
	const unsigned BUFLEN = 256;
	char buf[BUFLEN];

	std::string data;

	ssize_t bytesReceived = 0;
	int flags = 0; // first recv is blocking

	while((bytesReceived = ::recv(m_socket, buf, BUFLEN, flags)) > 0) {
		data.append(buf, bytesReceived);
		flags = MSG_DONTWAIT; // switch to nonblocking to receive all the data
	}

	if(bytesReceived < 0 && errno != EAGAIN) {
		// there really was an error
		throw NetworkingException("TCPSocket", "recv() with MSG_DONTWAIT failed!");
	}

	return data;
}

void TCPSocket::close(void)
{
	::close(m_socket);
}

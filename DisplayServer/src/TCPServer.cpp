#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>

#include "Exception.h"

#include "TCPServer.h"

TCPServer::TCPServer(uint16_t port)
	: m_port(port)
{
}

TCPServer::~TCPServer()
{
	close(m_socket);
}

void TCPServer::start(void)
{
	// create the server socket
	if((m_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		throw NetworkingException("TCPServer", "Could not create listening socket!");
	}

	// server socket address
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(m_port);

	if(bind(m_socket, reinterpret_cast<struct sockaddr*>(&servaddr), sizeof(servaddr)) < 0) {
		throw NetworkingException("TCPServer", "Could not bind listening socket!");
	}

	// keep a maximum of 3 clients in the queue
	if(listen(m_socket, 3) < 0) {
		throw NetworkingException("TCPServer", "Could start listening on socket!");
	}
}

void TCPServer::stop(void)
{
	close(m_socket);
}

bool TCPServer::hasPendingConnection(void)
{
	fd_set rfds;
	struct timeval tv = {0, 0}; // don't wait on select

	FD_ZERO(&rfds);
	FD_SET(m_socket, &rfds);

	int result = select(m_socket+1, &rfds, NULL, NULL, &tv);

	if(result > 0) {
		// there is a pending connection
		return true;
	} else if(result < 0) {
		// there was an error
		throw NetworkingException("TCPServer", "select() on server socket failed!");
	}

	return false;
}

TCPSocket TCPServer::acceptConnection(void)
{
	int clientSocket = accept(m_socket, NULL, NULL);

	if(clientSocket < 0) {
		throw NetworkingException("TCPServer", "accept() failed!");
	}

	return TCPSocket(clientSocket);
}

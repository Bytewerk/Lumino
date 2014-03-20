#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <cstdint>

#include "TCPSocket.h"

class TCPServer {
	private:
		int      m_socket;
		uint16_t m_port;

	public:
		TCPServer(uint16_t port);
		~TCPServer();

		void start(void);
		void stop(void);

		bool hasPendingConnection(void);
		TCPSocket acceptConnection(void);
};

#endif // TCPSERVER_H

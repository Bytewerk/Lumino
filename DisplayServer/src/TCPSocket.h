#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>

class TCPSocket {
	private:
		int      m_socket;

	public:
		TCPSocket(int socket);
		TCPSocket(const std::string &addr, uint16_t port);
		~TCPSocket();

		void        send(const std::string &data);
		std::string recv(void);
};

#endif // TCPSOCKET_H

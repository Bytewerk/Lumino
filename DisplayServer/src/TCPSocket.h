#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <string>

class TCPSocket {
	private:
		int      m_socket;

	public:
		TCPSocket(void);
		TCPSocket(int socket);
		TCPSocket(const std::string &addr, uint16_t port);
		TCPSocket(const TCPSocket &s);
		~TCPSocket();

		void        send(const std::string &data) const;
		std::string recv(void) const;

		void        close(void);

		int getFileDescriptor(void) const { return m_socket; }

		TCPSocket& operator = (const TCPSocket &s)
		{
			m_socket = s.m_socket;
			return *this;
		}
};

#endif // TCPSOCKET_H

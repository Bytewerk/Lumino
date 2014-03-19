#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <sstream>

#include <stdint.h>

class Exception
{
	protected:
		std::string m_module;
		std::string m_message;

	public:
		Exception(const std::string &module, const std::string &message)
			: m_module(module), m_message(message)
		{
		}

		virtual const std::string& module()  const { return m_module; }
		virtual const std::string& message() const { return m_message; }
		virtual const char*        type()    const { return "Exception"; }
};

class IOException : public Exception
{
	public:
		IOException(const std::string &module, const std::string &message)
			: Exception(module, message)
		{
		}

		virtual const char* type() const { return "IOException"; }
};

class NetworkingException : public Exception
{
	public:
		NetworkingException(const std::string &module, const std::string &message)
			: Exception(module, message)
		{
		}

		virtual const char* type() const { return "NetworkingException"; }
};

class FreeTypeException : public Exception
{
	public:
		FreeTypeException(const std::string &module, const std::string &message, int errorCode)
			: Exception(module, message)
		{
			std::ostringstream oss;
			oss << message << " [errorCode: " << errorCode << "]";
			m_message = oss.str();
		}

		virtual const char* type() const { return "FreeTypeException"; }
};

#endif // EXCEPTION_H
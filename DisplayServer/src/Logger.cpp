#include <string>
#include <cstdio>
#include <malloc.h>

#include <sys/time.h>

#include "Logger.h"

// define constants
const char *Logger::STR_FATAL = "FATAL";
const char *Logger::STR_ERR		= "ERROR";
const char *Logger::STR_WARN	= "WARN ";
const char *Logger::STR_INFO	= "INFO ";
const char *Logger::STR_DEBUG = "DEBUG";
const char *Logger::STR_DUMP	= "DUMP ";

const char *Logger::COLOR_FATAL = "\033[1;31m";
const char *Logger::COLOR_ERR		= "\033[1;31m";
const char *Logger::COLOR_WARN	= "\033[1;33m";
const char *Logger::COLOR_INFO	= "\033[1;32m";
const char *Logger::COLOR_DEBUG = "\033[1m";
const char *Logger::COLOR_DUMP	= "\033[1;30m";
const char *Logger::COLOR_NONE	= "\033[0m";

const int Logger::LVL_FATAL =		0;
const int Logger::LVL_ERR		=		5;
const int Logger::LVL_WARN	=  50;
const int Logger::LVL_INFO	= 100;
const int Logger::LVL_DEBUG = 200;
const int Logger::LVL_DUMP	= 500;

Logger::Logger()
	: useColors(true), verbosity(2147483647)
{
	// Initialize the semaphore
	sem_init(&semaphore, 0, 1);
}

Logger::~Logger()
{
	sem_destroy(&semaphore);
}

void Logger::EnableColors(bool enable)
{
	useColors = enable;
}

void Logger::SetVerbosity(int verbosity)
{
	this->verbosity = verbosity;
}

void Logger::DebugMessage(const char *prefix, const char *module,
		const char *fmt, va_list ap)
{
	/* Guess we need no more than 100 bytes. */
	int n, size = 100;
	char *p, *np;
	va_list internal_ap;

	if ((p = (char*)malloc(size)) == NULL)
	{
		fprintf(stderr, "[%s] FATAL: Cannot allocate string buffer while processing arguments.\n", STR_ERR);
		return;
	}

	while (1) {
		/* Try to print in the allocated space. */
		va_copy(internal_ap, ap);
		n = vsnprintf(p, size, fmt, internal_ap);
		va_end(internal_ap);

		/* If that worked, return the string. */
		if (n > -1 && n < size)
			break;

		/* Else try again with more space. */
		if (n > -1)		 /* glibc 2.1 */
			size = n+1; /* precisely what is needed */
		else					 /* glibc 2.0 */
			size *= 2;	/* twice the old size */

		if ((np = (char*)realloc (p, size)) == NULL)
		{
			free(p);
			fprintf(stderr, "[%s] FATAL: Cannot reallocate string buffer while processing arguments.\n", STR_ERR);
			return;
		}
		else
		{
			p = np;
		}
	}

	fprintf(stderr, "%s %s: %s\n", prefix, module, p);

	free(p);
}

void Logger::Log(int level, const char *module, const char *format, ...)
{
	va_list argptr;

	char timebuf[32];
	char timebuf2[32];

	if(level > verbosity)
		return;

	sem_wait(&semaphore);

	timeval tv;
	gettimeofday(&tv, NULL);
	strftime(timebuf, 32, "%Y-%m-%d %H:%M:%S.%%03d", localtime(&(tv.tv_sec)));
	snprintf(timebuf2, 32, timebuf, tv.tv_usec/1000);

	std::string prefix = std::string(timebuf2) + " [";

	if(level >= LVL_DUMP)
	{
		if(useColors)
			prefix += COLOR_DUMP;

		prefix += STR_DUMP;
	}
	else if(level >= LVL_DEBUG)
	{
		if(useColors)
			prefix += COLOR_DEBUG;

		prefix += STR_DEBUG;
	}
	else if(level >= LVL_INFO)
	{
		if(useColors)
			prefix += COLOR_INFO;

		prefix += STR_INFO;
	}
	else if(level >= LVL_WARN)
	{
		if(useColors)
			prefix += COLOR_WARN;

		prefix += STR_WARN;
	}
	else if(level >= LVL_ERR)
	{
		if(useColors)
			prefix += COLOR_ERR;

		prefix += STR_ERR;
	}
	else
	{
		if(useColors)
			prefix += COLOR_FATAL;

		prefix += STR_FATAL;
	}

	if(useColors)
		prefix += COLOR_NONE;

	prefix += "]";

	va_start(argptr, format);
	DebugMessage(prefix.c_str(), module, format, argptr);
	va_end(argptr);

	sem_post(&semaphore);
}

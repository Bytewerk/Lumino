#ifndef UTILS_LOGGER_H
#define UTILS_LOGGER_H

#include <cstdarg>

#include <semaphore.h>

/*!
 * \brief A Logger class.
 *
 * This class provides a consistent logging interface with verbosity
 * management. Each logged message has a priority level (lower values are
 * more important).
 *
 * The verbosity can be controlled by setting a maximum priority level.
 * Messages which have a higher level are simply ignored and not printed.
 *
 * Additionally, messages are prefixed with a string indicating their level,
 * which can be optionally colored.
 */
class Logger
{
	private:
		// *** Constants
		static const char *STR_FATAL;
		static const char *STR_ERR;
		static const char *STR_WARN;
		static const char *STR_INFO;
		static const char *STR_DEBUG;
		static const char *STR_DUMP;

		static const char *COLOR_FATAL;
		static const char *COLOR_ERR;
		static const char *COLOR_WARN;
		static const char *COLOR_INFO;
		static const char *COLOR_DEBUG;
		static const char *COLOR_NONE;
		static const char *COLOR_DUMP;

		// *** Attributes
		bool useColors;
		int  verbosity;

		sem_t semaphore;

		// *** Private Methods
		// This is a Singleton-Class!
		Logger();

		// Internal method for printing the output
		void DebugMessage(const char *prefix, const char *module,
				const char *fmt, va_list ap);

	public:
		// *** Constants
		static const int LVL_FATAL; /*!< Fatal message level */
		static const int LVL_ERR;		/*!< Error message level */
		static const int LVL_WARN;	/*!< Warning message level */
		static const int LVL_INFO;	/*!< Information message level */
		static const int LVL_DEBUG; /*!< Debug message level */
		static const int LVL_DUMP;	/*!< Dump message level */

		// *** Methods
		~Logger();

		/*!
		 * Log a message with given level and format (as in printf)
		 *
		 * \param level    Level of the message (see LVL_* constants for possible
		 *                 values).
		 * \param module   The name of the module that generated the message
		 * \param format   Format string (similar to printf)
		 * \param ...      Additional parameters, depending on format
		 */
		void Log(int level, const char *module, const char *format, ...);

		/*!
		 * Enable or disable output coloring.
		 */
		void EnableColors(bool enable);

		/*!
		 * Set the output verbosity.
		 *
		 * \param verbosity		Maximum message level to be printed.
		 */
		void SetVerbosity(int verbosity);

		/*!
		 * Get the one and only instance of Logger.
		 *
		 * \returns A reference to the Logger instance.
		 */
		static Logger& Instance()
		{
			static Logger theLogger;
			return theLogger;
		}
};

#define LOG(level, module, ...) Logger::Instance().Log(level, module, __VA_ARGS__)

#endif // UTILS_LOGGER_H

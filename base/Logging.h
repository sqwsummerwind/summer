//2016.9.10
//qiangwei.su
//

#ifndef SUMMER_BASE_LOGGING_H
#define SUMMER_BASE_LOGGING_H

#include <ostream>

#include <boost/scoped_ptr.hpp>

namespace summer
{
	class LoggingImpl;
	class Logger
	{
		public:
			enum LogLevel
			{
				TRACE,
				DEBUG,
				INFO,
				WARN,
				ERROR,
				FATAL,
				NUM_LOG_LEVELS
			};

			Logger(const char* file,  int line);
			Logger(const char* file,  int line, LogLevel loglevel);
			Logger(const char* file,  int line, LogLevel loglevel, const char* func);
			Logger(const char* file,  int line, bool ToAbort);
			~Logger();

			std::ostream& stream();

			static void setLogLevel(LogLevel logLevel);
			static LogLevel logLevel();

		private:
			boost::scoped_ptr<LoggingImpl> impl_;
	};

	#define LOG_INFO Logger(__FILE__, __LINE__).stream()
	#define LOG_TRACE if(Logger::logLevel() <= Logger::TRACE) Logger(__FILE__, __LINE__, Logger::TRACE, __func__).stream()
	#define LOG_DEBUG Logger(__FILE__,__LINE__,Logger::DEBUG,__func__).stream()
	#define LOG_WARN Logger(__FILE__,__LINE__,Logger::WARN).stream()
	#define LOG_ERROR Logger(__FILE__,__LINE__,Logger::ERROR).stream()
	#define LOG_FATAL Logger(__FILE__,__LINE__,Logger::FATAL).stream()
	#define LOG_SYSERR Logger(__FILE__,__LINE__,false).stream()
	#define LOG_SYSFATAL Logger(__FILE__,__LINE__,true).stream()

	const char* strerror_tl(int savedErrno);
}

#endif



//2016.910
//qiangwei.su
//
#include <errno.h>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "Timestamp.h"
#include "Logging.h"
#include "Thread.h"

namespace summer
{
	class LoggingImpl
	{
		public:
			typedef Logger::LogLevel LogLevel;
			LoggingImpl(const char* file, int line, LogLevel logLevel, int old_errno);
			
			void finish();

			const char* basename_;
			const char* fullname_;
			int line_;
			LogLevel logLevel_;
			Timestamp timestamp_;
			const char* function_;
			std::ostringstream stream_;
			
			static const char* logLevelName[];

	};
	
	__thread char t_errorStrbuf[512];

	const char* strerror_tl(int savedErrno)
	{
		return strerror_r(savedErrno,t_errorStrbuf, sizeof(t_errorStrbuf));
	}

	Logger::LogLevel initLogLevel()
	{
		if(::getenv("SUMMER_LOG_TRACE"))
		{
			return Logger::TRACE;
		}else
		{
			return Logger::DEBUG;
		}
	}

	Logger::LogLevel g_logLevel = initLogLevel();

}//end summer
	
using namespace summer;

const char* LoggingImpl::logLevelName[Logger::NUM_LOG_LEVELS] = 
{
	"TRACE",
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"FATAl",
};

LoggingImpl::LoggingImpl(const char* file, int line, Logger::LogLevel logLevel, int savedErrno)
:basename_(NULL),
fullname_(file),
line_(line),
logLevel_(logLevel),
timestamp_(Timestamp::now()),
function_(NULL),
stream_()
{
	const char* index = strrchr(const_cast<char*>(fullname_), '/');
	basename_ = (index != NULL) ? index + 1 : fullname_;
	char message_head[512];
	snprintf(message_head, sizeof(message_head), "%s %5d %s ", 
					timestamp_.toFormatString().c_str(),
					CurrentThread::tid(), logLevelName[logLevel_]);
	stream_ << message_head;
	if(savedErrno != 0)
	{
		stream_ << strerror_tl(savedErrno) << "(errno = " << savedErrno <<") "; 
	}
}



void LoggingImpl::finish()
{
	stream_ <<" - "<< basename_ << " : " << line_ <<"\n";
}

std::ostream& Logger::stream()
{
	return impl_->stream_;
}

//for INFO
Logger::Logger(const char* file, int line):
	impl_(new LoggingImpl(file, line, INFO, 0))
	{}	

//for WARN
Logger::Logger(const char* file, int line, Logger::LogLevel logLevel):
	impl_(new LoggingImpl(file, line, logLevel, 0))
	{}
	
//for DEBUG, TRACE
Logger::Logger(const char* file, int line, Logger::LogLevel logLevel, const char* function):
	impl_(new LoggingImpl(file, line, logLevel, 0))
	{
		impl_->function_ = function;
		impl_ ->stream_ << function << " ";

	}

//for FATAL, ERROR
Logger::Logger(const char* file, int line, bool toAbort):
	impl_(new LoggingImpl(file, line, toAbort?Logger::FATAL : Logger::ERROR, errno))
	{}

Logger::~Logger()
{
	impl_->finish();
	std::string message(impl_->stream_.str());
	int n = ::write(1, message.data(), message.size());
	(void)n;
	if(impl_->logLevel_ == Logger::FATAL)
	{
		abort();
	}

}

void Logger::setLogLevel(Logger::LogLevel logLevel)
{
	g_logLevel = logLevel;
}

Logger::LogLevel Logger::logLevel()
{
	return g_logLevel;
}







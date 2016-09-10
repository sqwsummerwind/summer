#include <stdio.h>
#include <sys/time.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

#include <boost/static_assert.hpp>

#include "Timestamp.h"

using namespace summer;

BOOST_STATIC_ASSERT(sizeof(Timestamp) == sizeof(int64_t));

//invalid timestamp
Timestamp::Timestamp()
:microSecondSinceEpoch_(0)
{
}

//timestamp with microsecond 
Timestamp::Timestamp(int64_t microSecondSinceEpoch) 
:microSecondSinceEpoch_(microSecondSinceEpoch)
{
}

//return format like this: seconds.xxxxx, how many seconds
std::string Timestamp::toString() const
{
	char buf[32] = {0};
	int64_t seconds = microSecondSinceEpoch_ / kMicroSecondPerSecond;
	int64_t microseconds = microSecondSinceEpoch_ % kMicroSecondPerSecond;
	snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
	return buf;
}

//return format 
std::string Timestamp::toFormatString() const
{
	char buf[32] = {0};

	time_t seconds = static_cast<time_t>(microSecondSinceEpoch_ / kMicroSecondPerSecond);
	int micorseconds = static_cast<int>(microSecondSinceEpoch_ % kMicroSecondPerSecond);

	struct tm time_st;
	gmtime_r(&seconds, &time_st);

	snprintf(buf, sizeof(buf)-1, "%4d-%02d-%02d %02d:%02d:%02d.%06d", time_st.tm_year + 1900, time_st.tm_mon + 1, time_st.tm_mday, time_st.tm_hour, time_st.tm_min, time_st.tm_sec, micorseconds);
	return buf;
}

Timestamp Timestamp::now()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	//time_t tv_sec;
	int64_t seconds =  tv.tv_sec;
	int64_t microseconds = tv.tv_usec;

	return Timestamp(seconds * kMicroSecondPerSecond + microseconds);
}

Timestamp Timestamp::invalid()
{
	return Timestamp();
}

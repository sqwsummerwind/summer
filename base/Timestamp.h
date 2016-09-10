//2016.9.9
//qiangwei.su
//
#ifndef SUMMER_BASE_TIMESTAMP_H
#define SUMMER_BASE_TIMESTAMP_H
#include <string.h>
#include <stdint.h>
#include <boost/operators.hpp>

namespace summer
{
	class Timestamp : public boost::less_than_comparable<Timestamp>
	{
		public:
			//
			//for invalid timstamp
			//
			Timestamp();

			//create timestamp at explicit time
			explicit Timestamp(int64_t microSecondSinceEpoch);

			//swap two timestamp
			void swap(Timestamp& that)
			{
				std::swap(microSecondSinceEpoch_, that.microSecondSinceEpoch_);
			}

			bool valid() const
			{
				return microSecondSinceEpoch_ > 0;
			}

			//get microSecondSinceEpoch_
			int64_t microSecondSinceEpoch()
			{
				return microSecondSinceEpoch_;
			}

			//get time of now
			static Timestamp now();

			//get invalid timestamp
			static Timestamp invalid();

			std::string toString() const;

			std::string toFormatString() const;

			static const int kMicroSecondPerSecond = 1000 * 1000;
		
		private:
			int64_t microSecondSinceEpoch_;

	};

	//just operator <, operator > ,>= ,<= will generate auto
	inline bool operator<(Timestamp& lt, Timestamp& rt)
	{
		return lt.microSecondSinceEpoch() < rt.microSecondSinceEpoch();
	}

	inline bool operator==(Timestamp& lt, Timestamp& rt)
	{
		return lt.microSecondSinceEpoch() == rt.microSecondSinceEpoch();
	}

}

#endif


//2016.9.11
//qiangwei.su

#ifndef SUMMER_NET_POLLER_H
#define SUMMER_NET_POLLER_H
#include <vector>
#include <map>
#include <poll.h>

#include <boost/noncopyable.hpp>

#include "../base/Timestamp.h"
//#include "EventLoop.h"

namespace summer
{
	namespace net
	{	
		class Channel;
		//struct pollfd;
		class EventLoop;

		class Poller: boost::noncopyable
		{
			public:
				typedef std::vector<Channel*> ChannelList;

				Poller(EventLoop* loop);
				~Poller();

				Timestamp poll(int timeousMs, ChannelList* activeChannels);

				void updateChannel(Channel* channel);

				void assertInLoopThread();

			private:
				
				void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

				typedef std::vector<struct pollfd> PollFdList;
				typedef std::map<int, Channel*> ChannelMap;

				PollFdList pollfds_;
				ChannelMap channels_;
				EventLoop* ownerLoop_;

		};
	}
}


#endif

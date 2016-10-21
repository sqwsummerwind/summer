//2016.10.20
//qiangwei.su
//

#ifndef SUMMER_NET_POLLER_H
#define SUMMER_NET_POLLER_H

#include <vector>

#include <boost/noncopyable.hpp>

#include "../base/Timestamp.h"

namespace summer
{
namespace net
{
	class EventLoop;
	class Channel;

	class Poller: boost::noncopyable
	{
		public:
			typedef std::vector<Channel*> ChannelList;
	
			Poller(EventLoop* loop);
			virtual ~Poller();
			virtual Timestamp poll(int timeOutMs, ChannelList* activeChannels) = 0;
			virtual void updateChannel(Channel* channel) = 0;
			virtual void removeChannel(Channel* channel) = 0;
			static Poller* newDefaultPoller(EventLoop* loop);
			void assertInLoopThread();
		private:
			virtual void fillActiveChannels(int numEvents, ChannelList* activeChannels) const = 0;
			EventLoop* loop_;
	};
}
}

#endif



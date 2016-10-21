//2016.9.11
//qiangwei.su

#ifndef SUMMER_NET_POLLER_POLLPOLLER_H
#define SUMMER_NET_POLLER_POLLPOLLER_H
#include <map>
#include <poll.h>

#include "../Poller.h"

namespace summer
{
namespace net
{	
	class PollPoller: public Poller
	{
		public:
			PollPoller(EventLoop* loop);
			~PollPoller();

			virtual Timestamp poll(int timeousMs, ChannelList* activeChannels);

			virtual void updateChannel(Channel* channel);
			virtual void removeChannel(Channel* channel);

		private:
				
			virtual void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

			typedef std::vector<struct pollfd> PollFdList;
			typedef std::map<int, Channel*> ChannelMap;

			PollFdList pollfds_;
			ChannelMap channels_;

	};
}
}


#endif

//2016.10.20
//qiangwei.su
//

#ifndef SUMMER_NET_POLLER_EPOLLPOLLER_H
#define SUMMER_NET_POLLER_EPOLLPOLLER_H

#include <map>
#include <sys/epoll.h>

#include "../Poller.h"

namespace summer
{
namespace net
{
	class EpollPoller: public Poller
	{
		public:
			EpollPoller(EventLoop* loop);
			~EpollPoller();
	
			virtual Timestamp poll(int timeOutMs, ChannelList* activeChannels);
			virtual void updateChannel(Channel* channel);
			virtual void removeChannel(Channel* channel);
		private:
			virtual void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
			
			void update(int operation, Channel* channel);
			static const int InitMaxEvents = 20;
			int epfd;
			std::vector<struct epoll_event> events_;
			std::map<int, Channel*> channels_;
	};

}
}
#endif



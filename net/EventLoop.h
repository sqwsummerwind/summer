//2016.9.11
//qiangwei.su
//

#ifndef SUMMER_NET_EVENTLOOP_H
#define SUMMER_NET_EVENTLOOP_H

#include <vector>
#include <pthread.h>

#include "../base/Thread.h"

#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace summer
{
	namespace net
	{	
		class Poller;
		class Channel;

		class EventLoop : boost::noncopyable
		{
			public:
				EventLoop();
				~EventLoop();
				void loop();
				void assertInLoopThread()
				{
					if(!isInLoopThread())
					{
						assertNotInLoopThread();
					}
				}

				bool isInLoopThread()
				{
					return threadId_ == CurrentThread::tid();
				}
				
				static EventLoop* getEventOfCurrentThread();
				void updateChannel(Channel* channel);
				void quit();
			private:
				void assertNotInLoopThread();

				typedef std::vector<Channel*> ChannelList;

				ChannelList activeChannels_;
				boost::scoped_ptr<Poller> poller_;

				bool looping_;
				bool quit_;
				const pid_t threadId_;
		};
	}
}

#endif



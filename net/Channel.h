//2016.9.11
//qiangwei.su
//

#ifndef SUMMER_NET_CHANNEL_H
#define SUMMER_NET_CHANNEL_H
#include <boost/noncopyable.hpp>

#include "EventLoop.h"

#include "../base/Timestamp.h"

namespace summer
{
	namespace net
	{	
		class EventLoop;

		class Channel : boost::noncopyable
		{
			public:
				typedef boost::function<void()> EventCallback;
				typedef boost::function<void(Timestamp)> ReadEventCallback;

				static const int kNoneEvent;
				
				Channel(EventLoop* loop, int fd);
				~Channel();

				//handle event
				void handleEvent(Timestamp recetive);
				void setReadCallback(const ReadEventCallback& cb)
				{
					readCallback_ = cb;
				}

				void setWriteCallback(const EventCallback& cb)
				{
					writeCallback_ = cb;
				}

				void setErrorCallback(const EventCallback& cb)
				{
					errorCallback_ = cb;
				}

				void setCloseCallback(const EventCallback& cb)
				{
					closeCallback_ = cb;
				}

				void enableReading()
				{
					events_ |= kReadEvent;
					update();
				}
				void enableWriting()
				{
					events_ |= kWriteEvent;
					update();
				}
				void disableWriting()
				{
					events_ &= ~kWriteEvent;
					update();
				}
				void disableAll()
				{
					events_ = kNoneEvent;
					update();
				}

				bool isWriting() const
				{
					return events_&kWriteEvent;
				}
				
				int fd() const 
				{
					return fd_;
				}

				int events() const
				{
					return events_;
				}

				bool isNoneEvent()
				{
					return events_ == kNoneEvent;
				}

				void set_revents(int revents)
				{
					revents_ = revents;
				}
				//for poller
				int index()
				{
					return index_;
				}

				void set_index(int index)
				{
					index_ = index;
				}
				
				EventLoop* ownerLoop()
				{
					return loop_;
				}


			private:

				void update();

				static const int kReadEvent;
				static const int kWriteEvent;

				int events_;	//events pay attention to 
				int revents_;	//events ready
				int fd_;	//fd connect to channel
				EventLoop* loop_;
				int index_;	//used by poller
				bool eventHandling;

				ReadEventCallback readCallback_;
				EventCallback writeCallback_;
				EventCallback errorCallback_;
				EventCallback closeCallback_;
		};
	}
}


#endif



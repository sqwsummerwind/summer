//2016.9.11
//qiangwei.su
//
#include <poll.h>

#include "../base/Logging.h"
#include "Channel.h"
#include "EventLoop.h"

using namespace summer;
using namespace summer::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd)
: events_(0),
revents_(0),
fd_(fd),
loop_(loop),
index_(-1)
{}

void Channel::update()
{
	loop_->updateChannel(this);
}

void Channel::handleEvent()
{
	if(revents_ & POLLNVAL)
	{
		LOG_WARN <<"Channel::handleEvent() POLLNVAL";
	}

	if(revents_ &(POLLERR | POLLNVAL))
	{
		if(errorCallback_)
			errorCallback_();
	}

	if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		if(readCallback_)
			readCallback_();
	}

	if(revents_ & POLLOUT)
	{
		if(writeCallback_)
			writeCallback_();
	}

}



//2016.9.11
//qiangwei.su
//
#include <poll.h>
#include <assert.h>

#include "../base/Logging.h"
#include "Channel.h"
#include "EventLoop.h"

using namespace summer;
using namespace summer::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop* loop, int fd):
events_(0),
revents_(0),
fd_(fd),
loop_(loop),
index_(-1),
eventHandling(false)
{}

Channel::~Channel()
{
	assert(!eventHandling);
}

void Channel::update()
{
	loop_->updateChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime)
{
	eventHandling = true;
	if(revents_ & POLLNVAL)
	{
		LOG_WARN <<"Channel::handleEvent() POLLNVAL fd: "<< fd_ <<" ";
	}

	//peer client close 
	if((revents_&POLLHUP)&&!(revents_&POLLIN))
	{
		LOG_WARN<<"Channel::handle_event() POLLHUP";
		LOG_DEBUG<<" closeCallback receive time: "<<receiveTime.toFormatString();
		if(closeCallback_)
				closeCallback_();
	}

	if(revents_ &(POLLERR | POLLNVAL))
	{
		LOG_DEBUG<<" errorCallback receive time: "<<receiveTime.toFormatString();
		if(errorCallback_)
			errorCallback_();
	}

	if(revents_ & (POLLIN | POLLPRI | POLLRDHUP))
	{
		LOG_DEBUG<<" readCallback receive time: "<<receiveTime.toFormatString();
		if(readCallback_)
			readCallback_(receiveTime);
	}

	if(revents_ & POLLOUT)
	{
		LOG_DEBUG<<" writeCallback receive time: "<<receiveTime.toFormatString();
		if(writeCallback_)
			writeCallback_();
	}
	eventHandling = false;

}



//2016.9.11
//qiangwei.su
//
#include <poll.h>

#include "Poller.h"
#include "EventLoop.h"
#include "Channel.h"
#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

Poller::Poller(EventLoop* loop)
:ownerLoop_(loop)
{}

Poller::~Poller()
{}

void Poller::assertInLoopThread()
{
	ownerLoop_->assertInLoopThread();
}

void Poller::updateChannel(Channel* channel)
{
	//call in loop thread;
	assertInLoopThread();
	if(channel->index())
	{
		//add new channel
		assert(channels_.find(channel->fd()) == channels_.end());
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);
		int idx = static_cast<int>(pollfds_.size())-1;
		channel->set_index(idx);
		channels_[pfd.fd] = channel;
	}
	else
	{
		//update existing one
		assert(channels_.find(channel->fd()) != channels_.end());
		assert(channels_[channel->fd()] == channel);
		
		int idx = channel->index();
		assert(idx>=0&&idx < static_cast<int>(pollfds_.size()));
		struct pollfd& pfd = pollfds_[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == -1);
		pfd.events = channel->events();
		pfd.revents = 0;
		if(channel->isNoneEvent())
		{	
			//ignore this pollfd
			pfd.fd = -1;
		}
	}
}

Timestamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
	int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
	Timestamp now(Timestamp::now());
	if(numEvents > 0)
	{	
		LOG_TRACE<<numEvents<<"events happens";
		fillActiveChannels(numEvents, activeChannels);
	}
	else if(numEvents == 0)
	{
		LOG_TRACE<<"no events";
	}
	else
	{
		LOG_SYSERR << "Poller::poll()";
	}

	return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
	for(PollFdList::const_iterator it = pollfds_.begin();it!=pollfds_.end()&&numEvents>0;it++)
	{
		if(it->revents>0)
		{
			numEvents--;
			ChannelMap::const_iterator ch = channels_.find(it->fd);
			assert(ch!=channels_.end());
			Channel* channel = ch->second;
			assert(channel->fd()==it->fd);
			channel->set_revents(it->revents);
			activeChannels->push_back(channel);
		}
	}
}



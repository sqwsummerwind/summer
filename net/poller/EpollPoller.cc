//2016.10.20
//qiangwei.su
//

#include <string>
#include <strings.h>
#include <poll.h>

#include <boost/static_assert.hpp>

#include "EpollPoller.h"

#include "../EventLoop.h"
#include "../Channel.h"

#include "../../base/Logging.h"

using namespace summer;
using namespace summer::net;

BOOST_STATIC_ASSERT(EPOLLIN == POLLIN);
BOOST_STATIC_ASSERT(EPOLLOUT == POLLOUT);
BOOST_STATIC_ASSERT(EPOLLRDHUP == POLLRDHUP);
BOOST_STATIC_ASSERT(EPOLLPRI == POLLPRI);
BOOST_STATIC_ASSERT(EPOLLERR == POLLERR);
BOOST_STATIC_ASSERT(EPOLLHUP == POLLHUP);
//BOOST_STATIC_ASSERT(EPOLLET == POLLET);

const int EpollPoller::InitMaxEvents;

EpollPoller::EpollPoller(EventLoop* loop):
Poller(loop),
epfd(::epoll_create1(EPOLL_CLOEXEC)),
events_(InitMaxEvents)
{
	if(epfd < 0)
	{
		LOG_SYSFATAL<<"epoll_create1() error";
	}
}

EpollPoller::~EpollPoller()
{
	::close(epfd);
}


Timestamp EpollPoller::poll(int timeOutMs, ChannelList* activeChannels)
{
	int numEvents = epoll_wait(epfd,
					&*events_.begin(),
					static_cast<int>(events_.size()),
					timeOutMs);

	Timestamp now(Timestamp::now());
	
	if(numEvents > 0)
	{
		fillActiveChannels(numEvents, activeChannels);
		if(numEvents == static_cast<int>(events_.size()))
		{
			events_.resize(events_.size() * 2);
		}
	}
	else if(numEvents == 0)
	{
		LOG_TRACE<<" None events";
	}
	else if(numEvents < 0)
	{
		LOG_SYSERR<<" epoll_wait() error";
	}

	return now;
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
	assert(static_cast<size_t>(numEvents) <= events_.size());
	for(int i = 0; i < numEvents; ++i)
	{
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
		int fd = channel->fd();
		std::map<int, Channel*>::const_iterator it = channels_.find(fd);
		assert(it != channels_.end());
		assert(it->second == channel);
		assert(channel->index() == 1);
		channel->set_revents(events_[i].events);
		activeChannels->push_back(channel);
	}
}

void EpollPoller::updateChannel(Channel* channel)
{
	Poller::assertInLoopThread();
	
	if(channel->index() < 0)
	{
		//add new channel
		int fd = channel->fd();
		std::map<int, Channel*>::const_iterator it = channels_.find(fd);
		assert(it == channels_.end());
		channel->set_index(1);
		channels_[fd] = channel;
		update(EPOLL_CTL_ADD, channel);
	}
	else
	{
		//update one exist
		int fd = channel->fd();
		assert(channels_.find(fd) != channels_.end());
		assert(channels_[fd] == channel);
		assert(channel->index() == 1);
		update(EPOLL_CTL_MOD, channel);
	}
}

void EpollPoller::removeChannel(Channel* channel)
{	
	Poller::assertInLoopThread();

	int fd = channel->fd();
	assert(channels_.find(fd) != channels_.end());
	assert(channels_[fd] == channel);
	assert(channel->events() == Channel::kNoneEvent);
	channels_.erase(fd);
	update(EPOLL_CTL_DEL, channel);
}

void EpollPoller::update(int operation, Channel* channel)
{
	struct epoll_event ev;
	bzero(&ev, sizeof ev);
	ev.events = channel->events();
	ev.data.ptr = channel;
	if(::epoll_ctl(epfd, operation, channel->fd(), &ev) < 0)
	{
		LOG_SYSFATAL<<"epoll_ctl() error operation: "<<operation;
	}
}

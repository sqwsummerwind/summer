//2016.10.20
//qiangwei.su
//

#include "PollPoller.h"
#include "EpollPoller.h"

#include "../Poller.h"

using namespace summer;
using namespace summer::net;


Poller* Poller::newDefaultPoller(EventLoop* loop)
{
	if(::getenv("SUMMER_POLL"))
	{
		return (new PollPoller(loop));
	} else {
		return (new EpollPoller(loop));
	}
}


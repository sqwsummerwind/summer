//2016.10.20
//qiangwei.su
//


#include "Poller.h"
#include "EventLoop.h"

using namespace summer;
using namespace summer::net;

Poller::Poller(EventLoop* loop):
loop_(loop)
{

}

Poller::~Poller()
{

}

void Poller::assertInLoopThread()
{
	loop_->assertInLoopThread();
}




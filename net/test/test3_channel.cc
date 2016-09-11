//2016.9.11
//qiangwei.su
//

#include <sys/timerfd.h>
#include <stdio.h>
#include <strings.h>

#include "EventLoop.h"
#include "Channel.h"

summer::net::EventLoop* g_loop;

void timeout()
{
	printf("Timet out\n");
	g_loop->quit();

}

int main()
{
	summer::net::EventLoop loop;
	g_loop = &loop;

	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	summer::net::Channel channel(&loop, timerfd);
	channel.setReadCallback(timeout);
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong, sizeof(howlong));
	howlong.it_value.tv_sec = 3;
	::timerfd_settime(timerfd, 0, &howlong, NULL);

	loop.loop();
	::close(timerfd);
}


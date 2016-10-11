//2016.9.13
//qiangwei.su
//

#include <unistd.h>
#include <stdio.h>

#include "EventLoopThread.h"
#include "EventLoop.h"

#include <boost/bind.hpp>

using namespace summer;
using namespace summer::net;

void print(EventLoop* p = NULL)
{
	printf("pid: %d, tid:%d, loop:%p\n", ::getpid(), CurrentThread::tid(), p);
}

int main()
{
	print();

	{
		EventLoopThread etr1;
	}

	{
		EventLoopThread etr2;
		EventLoop* loop = etr2.startLoop();
		loop ->runInLoop(boost::bind(print, loop));
	}

	sleep(2);

	return 0;
}

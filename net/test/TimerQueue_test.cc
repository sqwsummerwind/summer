//2016.9.13
//qiangwei.su

#include <stdio.h>

#include "EventLoop.h"
#include "TimerQueue.h"
#include "TimerId.h"

#include <boost/bind.hpp>

#include "../base/Timestamp.h"
#include "../base/Thread.h"

using namespace summer;
using namespace summer::net;

EventLoop* g_loop;
int cnt = 0;
void print(const char* msg)
{
	printf("msg %s %s \n", Timestamp::now().toFormatString().c_str(), msg);
	if(++cnt == 10)
	{
		g_loop->quit();
	}
}

void printTid()
{
	printf("pid:%d tid:%d\n", ::getpid(), CurrentThread::tid());
	printf("now %s\n", Timestamp::now().toFormatString().c_str());
}

void cancel(TimerId timerId)
{
	g_loop->cancelTimer(timerId);
	printf("cancel at %s\n", Timestamp::now().toFormatString().c_str());
}

int main()
{
	print("main");
	{	
		EventLoop loop;
		g_loop = &loop;

		loop.runAfter(1, boost::bind(print, "once1"));
		loop.runAfter(1.5, boost::bind(print, "once2"));
		loop.runAfter(2.5, boost::bind(print, "once3"));
		loop.runAfter(3.5, boost::bind(print, "once4"));
		TimerId t45 = loop.runAfter(4.5, boost::bind(print, "once5"));

		loop.runAfter(4.2, boost::bind(cancel, t45));
		//loop.runAfter(4.8, boost::bind(cancel, t45));

		TimerId t3 = loop.runEvery(3, boost::bind(print, "every"));
		loop.runAfter(8.999, boost::bind(cancel, t3));

		loop.loop();
		printf("loop quit\n");
	}

	return 0;
}


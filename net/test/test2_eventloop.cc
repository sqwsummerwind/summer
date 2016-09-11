//2016.9.11
//qiangwei.su
//
//

#include "EventLoop.h"

summer::net::EventLoop* g_loop;

void ThreadFunc()
{
	g_loop->loop();
}

int main()
{
	summer::net::EventLoop loop;
	g_loop = &loop;
	summer::Thread thread(ThreadFunc);
	thread.start();
	thread.join();
	return 0;
}


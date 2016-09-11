//2016.9.11
//qiangwei.su
//

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "EventLoop.h"


void ThreadFunc()
{	
	printf("ThreadFunc: pid = %d, tid = %d\n",
					getpid(), summer::CurrentThread::tid());
	summer::net::EventLoop loop;
	loop.loop();
}

int main()
{
	printf("main: pid = %d, tid = %d \n",
					getpid(), summer::CurrentThread::tid());
	
	summer::Thread thread(ThreadFunc);
	thread.start();
	thread.join();

	summer::net::EventLoop loop;
	loop.loop();
}



//2016.9.10
//qiangwei.su

#include <stdio.h>

#include <boost/bind.hpp>

#include "Thread.h"

void ThreadFunc1()
{
	printf("thread free function 1, tid:%d\n", summer::CurrentThread::tid());
}

void ThreadFunc2(int x)
{
	printf("thread free function 2, tid:%d, x:%d\n", summer::CurrentThread::tid(), x);
	printf("thread name:%s\n", summer::CurrentThread::threadName());
}

class Foo
{
	public:
		explicit Foo(double x):x_(x){}

		void memberFun1()
		{
			printf("foo member function 1, tid:%d, x_:%f\n", summer::CurrentThread::tid(), x_);
		}

		void memberFun2(std::string& text)
		{
			printf("foo member function 2, tid:%d, text:%s, x_:%f\n", summer::CurrentThread::tid(), text.c_str(), x_);
		}
	private:
		double x_;
};

int main()
{	
	printf("tid:%d, pid:%d\n", summer::CurrentThread::tid(), ::getpid());

	summer::Thread t1(ThreadFunc1);
	t1.start();
	t1.join();

	summer::Thread t2(boost::bind(&ThreadFunc2, 90), "thead2");
	t2.start();
	t2.join();

	Foo foo(10);
	summer::Thread t3(boost::bind(&Foo::memberFun1, &foo), "thread3");
	t3.start();
	t3.join();

	summer::Thread t4(boost::bind(&Foo::memberFun2, boost::ref(foo), std::string("qiangwei.su")));
	t4.start();
	t4.join();

	printf("num created:%d\n", summer::Thread::numCreated());

}

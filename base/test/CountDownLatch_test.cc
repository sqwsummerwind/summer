//2016.9.9
//qiangwei.su
//

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#include "CountDownLatch.h"
using namespace summer;

CountDownLatch countLatch(5);
void* threadFun1(void *agr)
{
	printf("this is thread1\n");
	countLatch.wait();
	printf("count down latch in thread1\n");

	pthread_exit(0);
}

void* threadFun2(void *agr)
{
	printf("this is thread2\n");
	while(countLatch.getCount()>0)
	{
		printf("%d:count down in thread 2\n", countLatch.getCount());
		countLatch.countDown();
		sleep(1);
	}
	pthread_exit(0);
}

int main()
{
	pthread_t pid1;
	pthread_t pid2;

	pthread_create(&pid1, NULL, threadFun1, NULL);

	pthread_create(&pid2, NULL, threadFun2, NULL);

	pthread_join(pid1, NULL);
	pthread_join(pid2, NULL);

	return 0;

}



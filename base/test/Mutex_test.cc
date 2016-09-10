#include <stdio.h>

#include "../Mutex.h"

int a = 10;
summer::MutexLock mutexlock;

int main(){
	
	{
		summer::MutexLockGuard lock(mutexlock);

		a = 12;
	}
	printf("a=%d\n",a);
	return 0;

}



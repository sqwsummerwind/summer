#include <stdio.h>

#include "Timestamp.h"

using namespace summer;

void passByValue(Timestamp time)
{
	printf("pass by value:%s\n",time.toFormatString().c_str());
}

void passByRef(Timestamp& time)
{
	printf("pass by reference:%s\n", time.toFormatString().c_str());
}

int main()
{
	Timestamp ts(Timestamp::now());
	
	printf("time:%s\n", ts.toString().c_str());
	printf("time:%s\n", ts.toFormatString().c_str());
	
	passByValue(ts);
	passByRef(ts);
	return 0;
}



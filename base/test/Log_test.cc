//2016.9.10
//qiangwei.su
//

#include "Logging.h"

using namespace summer;

int main()
{	
	Logger logger(__FILE__, __LINE__);
	LOG_INFO << "this is logging test";
	LOG_DEBUG << "this is debug";
	LOG_WARN << "THIS IS WARN";
	LOG_ERROR<< "THIS IS ERROR";
	LOG_FATAL << "THIS IS FATAL";
	LOG_SYSERR << "THIS IS SYSTEM ERROR";
	LOG_SYSFATAL << "THIS IS SYSTEM FATAL";
	return 0;
	//LOG_TRACE;
}


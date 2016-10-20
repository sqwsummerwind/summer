//2016.10.12
//qiangwei.su
//

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include "InetAddress.h"
#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

int main(int argc, char* argv[])
{
	LOG_INFO<<"pid = "<<getpid();
	LOG_TRACE<<" main function ";

	InetAddress peerAddr(argv[1], 2016);
	LOG_INFO<<"peeraddress: "<<peerAddr.toHostPort();
	return 0;
}



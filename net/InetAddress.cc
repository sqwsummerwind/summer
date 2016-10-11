//2016.9.21
//qiangwei.su

#include <strings.h>

#include "InetAddress.h"
#include "SocketOpts.h"

using namespace summer;
using namespace summer::net;

InetAddress::InetAddress(uint16_t port)
{
	bzero(&addr_, sizeof(addr_));
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = sockets::hostToNetwork32(INADDR_ANY);
	addr_.sin_port = sockets::hostToNetwork16(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
	bzero(&addr_, sizeof(addr_));
	sockets::fromHostPort(ip.c_str(), port, &addr_);
}

std::string InetAddress::toHostPort() const 
{
	char buf[32];
	sockets::toHostPort(buf, sizeof(buf), addr_);
	return buf;
}



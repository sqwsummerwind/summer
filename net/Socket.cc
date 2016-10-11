//2016.9.21
//qiangwei.su

#include "Socket.h"
#include "InetAddress.h"
#include "SocketOpts.h"

#include <strings.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

using namespace summer;
using namespace summer::net;

Socket::~Socket()
{
	sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& listenAddr)
{
	sockets::bindOrDie(sockfd_, listenAddr.getInetAddr());
}

void Socket::listen()
{
	sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peerAddr)
{
	
	//return socket::accept(sockfd_,& (peerAddr->getInetAddr()));
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	int connfd = sockets::accept(sockfd_, &addr);
	if(connfd>0)
	{
		peerAddr->setInetAddr(addr);
	}

	return connfd;
}

void Socket::shutdownWrite()
{
	sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::setReuseAddr(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}







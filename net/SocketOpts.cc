//2016.9.21
//qiangwei.su

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <strings.h>
#include <stdio.h>
#include <errno.h>

#include <boost/implicit_cast.hpp>

#include "SocketOpts.h"

#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

namespace 
{
	typedef struct sockaddr SA;
	
	const SA* sockaddr_cast(const struct sockaddr_in* addr)
	{
		return static_cast<const SA*>(boost::implicit_cast<const void*>(addr));
	}

	SA* sockaddr_cast(struct sockaddr_in* addr)
	{
		return static_cast<SA*>(boost::implicit_cast<void*>(addr));
	}

	void setNonBlockAndCloseOnExec(int sockfd)
	{
		//non block
		int flags = ::fcntl(sockfd, F_GETFL, 0);
		flags |= O_NONBLOCK;
		int ret = ::fcntl(sockfd, F_SETFL, flags);

		//close-on-exec
		flags = ::fcntl(sockfd, F_GETFD, 0);
		flags |= FD_CLOEXEC;
		ret = ::fcntl(sockfd, F_SETFD, flags);
	}
}


int sockets::createNonblockingOrDie()
{
	//socket
	int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0)
	{
		LOG_SYSFATAL<<"sockets::createNonblockingOrDie()";
	}
	setNonBlockAndCloseOnExec(sockfd);
	return sockfd;
}

int sockets::connect(int sockfd, const struct sockaddr_in& addr)
{
	return ::connect(sockfd, sockaddr_cast(&addr), sizeof(addr));
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in& addr)
{
	int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
	if(ret<0)
	{
		LOG_SYSFATAL<<"sockets::bindOrDie()";
	}
}

void sockets::listenOrDie(int sockfd)
{
	int ret = ::listen(sockfd, SOMAXCONN);
	if(ret <0)
	{
		LOG_SYSFATAL<<"sockets::listenOrDie";
	}
}

int sockets::accept(int sockfd, struct sockaddr_in* addr)
{
	socklen_t addrlen = sizeof(*addr);
#if VALGRIND
	int connfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
	setNonBlockAndCloseOnExec(sockfd);
#else
	int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen,
					SOCK_NONBLOCK|SOCK_CLOEXEC);
#endif
	if(connfd<0)
	{
		int savedErrno = errno;
		LOG_SYSERR<<"sockets::accept";
		switch(savedErrno)
		{
			case EAGAIN:
			case ECONNABORTED:
			case EINTR:
			case EPROTO:
			case EPERM:
				break;
					
			case EBADF:
			case EFAULT:
			case EINVAL://sockfd not listening
			case EMFILE:
			case ENFILE:
			case ENOBUFS:
			case ENOMEM:
			case ENOTSOCK:
			case EOPNOTSUPP:
				LOG_FATAL<<"unexpected error of ::accept";
				break;
			default:
				LOG_FATAL<<"unknown error of ::accept";
				break;

		}
	}//end if

	return connfd;
}

void sockets::close(int sockfd)
{
	int ret = ::close(sockfd);
	if(ret<0)
	{
		LOG_FATAL<<"sockets::close";
	}
}

void sockets::shutdownWrite(int sockfd)
{
	int ret = ::shutdown(sockfd, SHUT_WR);
	if(ret<0)
	{
		LOG_FATAL<<"sockets::shutdownWrite";
	}
}

void sockets::toHostPort(char* buf, size_t size,
				const struct sockaddr_in& addr)
{
	char host[INET_ADDRSTRLEN] = "INVALID";
	//network fomat to 1.2.3.4
	::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
	uint16_t port = networkToHost16(addr.sin_port);
	snprintf(buf, size, "%s:%u", host, port);
}

void sockets::fromHostPort(const char* ip, uint16_t port,
				struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = hostToNetwork16(port);
	if(::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
	{
		LOG_SYSERR<<"socket::fromHostPort";
	}
}

int sockets::getSocketError(int sockfd)
{
	int optval;
	socklen_t optlen = sizeof(optval);

	if(::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen)<0)
	{
		return errno;
	}
	else
	{
		return optval;
	}
}

struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
	struct sockaddr_in localaddr;
	bzero(&localaddr, sizeof(localaddr));
	socklen_t addrlen = sizeof(localaddr);
	if(::getsockname(sockfd, sockaddr_cast(&localaddr),&addrlen)<0)
	{
		LOG_SYSERR<<"sockets::getLocalAddr";
	}
	return localaddr;
}

struct sockaddr_in sockets::getPeerAddr(int sockfd)
{
	struct sockaddr_in peeraddr;
	bzero(&peeraddr, sizeof(peeraddr));
	socklen_t addrlen = sizeof(peeraddr);
	if(::getpeername(sockfd, sockaddr_cast(&peeraddr),&addrlen)<0)
	{
		LOG_SYSERR<<"sockets::getPerrAddr";
	}
	
	return peeraddr;
}

bool sockets::isSelfConnect(int sockfd)
{
	struct sockaddr_in localaddr = getLocalAddr(sockfd);
	struct sockaddr_in peeraddr = getPeerAddr(sockfd);
	return localaddr.sin_port == peeraddr.sin_port &&
			localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}

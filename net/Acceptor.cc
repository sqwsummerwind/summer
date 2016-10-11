//2016.9.21
//qiangwei.su
//

#include <boost/bind.hpp>

#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include "SocketOpts.h"

using namespace summer;
using namespace summer::net;

Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr):
loop_(loop),
acceptSocket_(sockets::createNonblockingOrDie()),
acceptChannel_(loop, acceptSocket_.fd()),
listening_(false)
{
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.bindAddress(addr);
	acceptChannel_.setReadCallback(
					boost::bind(&Acceptor::handleRead, this));
}

void Acceptor::listen()
{
	loop_->assertInLoopThread();
	listening_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
	loop_->assertInLoopThread();
	InetAddress peerAddr(0);
	int connfd = acceptSocket_.accept(&peerAddr);
	if(connfd >= 0)
	{
		if(newConnectionCallback_)
		{
			newConnectionCallback_(connfd, peerAddr);
		}
		else
		{
			sockets::close(connfd);
		}
	}
}

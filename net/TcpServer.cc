//2016.9.23
//qiangwei.su
//

#include <assert.h>
#include <stdio.h>

#include <boost/bind.hpp>

#include "TcpServer.h"
#include "Acceptor.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "TcpConnection.h"
#include "SocketOpts.h"

#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& peerAddr, const std::string& name):
loop_(loop),
name_(name),
hostPort_(peerAddr.toHostPort()),
acceptor_(new Acceptor(loop, peerAddr)),
threadPool_(new EventLoopThreadPool(loop)),
started_(false),
nextConnId_(1)
{
	acceptor_->setNewConnectionCallback(
				boost::bind(&TcpServer::newConnection, this, _1, _2));
}

void TcpServer::setThreadNum(int numThreads)
{
	assert(numThreads>=0);
	threadPool_->setThreadNum(numThreads);
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
	LOG_TRACE<<" new connection ";
	//call getsocketname
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	
	char buf[32];
	snprintf(buf, sizeof(buf), ":%s-%d", hostPort_.c_str(), nextConnId_);
	nextConnId_++;
	std::string connName = name_ + buf;

	EventLoop* ioLoop = threadPool_->getNextLoop();
	TcpConnectionPtr conn(
			new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
	
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
			boost::bind(&TcpServer::removeConnection, this, _1));
	LOG_DEBUG<<"call ioLoop->runInLoop(boost::bind(&TcpConnection::connectionEstablished, conn))";
	ioLoop->runInLoop(boost::bind(&TcpConnection::connectionEstablished, conn));
	connections_[connName] = conn;
}

void TcpServer::start()
{
	loop_->assertInLoopThread();
	if(!acceptor_->listening())
	{
		acceptor_->listen();
	}

	if(!threadPool_->started())
	{
		threadPool_->start();
	}
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
	loop_->runInLoop(
			boost::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
	loop_->assertInLoopThread();
	LOG_INFO<<"TcpServer::removeConnectionInLoop ["<<name_
			<<"] - connection "<< conn->getName();
	size_t n = connections_.erase(conn->getName());
	assert(n == 1);
	(void)n;
	EventLoop* ioLoop = conn -> getLoop();
	ioLoop->queueInLoop(
			boost::bind(&TcpConnection::connectionDestroyed, conn));
}



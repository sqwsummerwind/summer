//2016.10.11
//qiangwei.su
//

#include <string>
#include "stdio.h"

#include <boost/bind.hpp>

#include "TcpClient.h"
#include "EventLoop.h"
#include "Connector.h"
#include "SocketOpts.h"
#include "TcpConnection.h"

#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

namespace summer
{
namespace net
{
	void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf, Timestamp timestamp)
	{
		LOG_TRACE<<"defaultMessageCallback";	
	}

	void defaultConnectionCallback(const TcpConnectionPtr& conn)
	{
		LOG_TRACE<<"defaultMessageCallback";
	}
}

}
TcpClient::TcpClient(EventLoop* loop, 
				const InetAddress& serverAddr,
				const std::string& name):
loop_(loop),
connector_(new Connector(loop, serverAddr)),
name_(name),
messageCallback_(defaultMessageCallback),
connectionCallback_(defaultConnectionCallback),
retry_(false),
connect_(true),
nextConnId_(1)
{
	connector_->setNewConnectionCallback(
				boost::bind(&TcpClient::newConnection, this, _1));
}

void TcpClient::connect()
{
	LOG_INFO<<"TcpClient::connection["<<name_<<"]"<<
			" connect to "<<connector_->serverAddress().toHostPort();
	connect_ = true;
	connector_->start();
}

//close the write side
void TcpClient::disconnect()
{
	LOG_INFO<<"TcpClient::disconnect["<<name_<<"]";
	connect_ = false;
	{
		MutexLockGuard lock(mutex_);
		if(connection_)
		{
			connection_->shutdown();
		}
	}
}

void TcpClient::newConnection(int sockfd)
{
	loop_->assertInLoopThread();
	//get the server address
	InetAddress peerAddr(sockets::getPeerAddr(sockfd));
	char buf[32];
	snprintf(buf, sizeof(buf), "%s#%d", peerAddr.toHostPort().c_str(), nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	TcpConnectionPtr conn(new TcpConnection(loop_, connName, sockfd, peerAddr, localAddr));

	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setConnectionCallback(connectionCallback_);
	conn->setCloseCallback(boost::bind(
				&TcpClient::removeConnection, this, _1));
	
	{
		MutexLockGuard lock(mutex_);
		connection_ = conn;
	}

	connection_->connectionEstablished();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn)
{
	loop_->assertInLoopThread();

	{
		MutexLockGuard lock(mutex_);
		assert(connection_ == conn);
		//connection_ -> connectionDestroyed();
		connection_.reset();
	}

	loop_->queueInLoop(boost::bind(
					&TcpConnection::connectionDestroyed, conn));

	if(retry_&&connect_)
	{
		LOG_INFO<<"TcpClient::connect["<<name_<<"]"<<
				" reconnect to "<<connector_->serverAddress().toHostPort();
		connector_->restart();
	}
}



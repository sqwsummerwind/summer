//2016.10.11
//qiangwei.su
//

#include <assert.h>
#include <errno.h>

#include <boost/bind.hpp>

#include "Connector.h"
#include "Channel.h"
#include "EventLoop.h"
#include "SocketOpts.h"

#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

const int Connector::kMaxRetryDelayMs;
const int Connector::kInitRetryDelayMs;

Connector::Connector(EventLoop* loop, const InetAddress& serverAddr):
loop_(loop),
state_(kDisConnected),
serverAddr_(serverAddr),
retryDelayMs_(kInitRetryDelayMs)
{

}

Connector::~Connector()
{
	assert(!channel_);
}


void Connector::start()
{
	loop_->runInLoop(boost::bind(&Connector::startInLoop, this));
}

void Connector::restart()
{
	setState(state_ = kDisConnected);
	retryDelayMs_ = kInitRetryDelayMs;
	startInLoop();
}

void Connector::startInLoop()
{
	loop_->assertInLoopThread();
	assert(state_ == kDisConnected);
	int sockfd = sockets::createNonblockingOrDie();
	int ret = sockets::connect(sockfd, serverAddr_.getInetAddr());
	int savedErrno = (ret==0) ? 0 : errno;
	switch(savedErrno)
	{
		case 0:
		case EINPROGRESS:
		case EINTR:
		case EISCONN:
			connecting(sockfd);
			break;

		case EAGAIN:
		case EADDRINUSE:
		case EADDRNOTAVAIL:
		case ECONNREFUSED:
		case ENETUNREACH:
			retry(sockfd);
			break;
		
		case EACCES:
		case EPERM:
		case EAFNOSUPPORT:
		case EALREADY:
		case EBADF:
		case EFAULT:
		case ENOTSOCK:
			LOG_SYSERR << "connect error in Connector::startInLoop " << savedErrno;
			sockets::close(sockfd);
			break;

		default:
			LOG_SYSERR << "Unexpected error in Connector::startInLoop " << savedErrno;
			sockets::close(sockfd);
			break;
	}
}


void Connector::connecting(int sockfd)
{
	//assert(state_ == kDisConnected);
	setState(kConnecting);
	assert(!channel_);
	channel_.reset(new Channel(loop_, sockfd));
	
	channel_->setWriteCallback(
			boost::bind(&Connector::handleWrite, this));
	channel_->setErrorCallback(
			boost::bind(&Connector::handleError, this));
	channel_->enableWriting();

}

//call by handleWrite
int Connector::removeAndResetChannel()
{
	channel_->disableAll();
	int sockfd = channel_->fd();
	loop_->removeChannel(channel_.get());
	loop_->runInLoop(
			boost::bind(&Connector::resetChannel, this));
	return sockfd;
}

void Connector::resetChannel()
{
	channel_.reset();
}

void Connector::handleWrite()
{
	LOG_TRACE<<"Connector::handleWrite";
	assert(state_ == kConnecting);
	int sockfd = removeAndResetChannel();
	//judge weather the connection is self connection
	if(sockets::isSelfConnect(sockfd))
	{
		LOG_WARN<<"Connector::handleWrite -- self connection";
		retry(sockfd);
	}
	else
	{
		setState(kConnected);
		newConnectionCallback_(sockfd);
	}

}

void Connector::handleError()
{
	LOG_TRACE<<"Connector::handleError";
	assert(state_ == kConnecting);

	int sockfd = removeAndResetChannel();
	int err = sockets::getSocketError(sockfd);
	LOG_TRACE << "error = " << err << " " << strerror_tl(err); 

	retry(sockfd);
}

void Connector::retry(int sockfd)
{
	sockets::close(sockfd);
	setState(state_ = kDisConnected);
	LOG_INFO<<"Connector::retry -- retry connecting to "<<
			serverAddr_.toHostPort() << " in "<<retryDelayMs_<<" millseconds";
	loop_->runAfter(static_cast<double>(retryDelayMs_)/1000, boost::bind(&Connector::startInLoop, this));
	retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
}

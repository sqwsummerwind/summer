//2016.9.22
//qiangwei.su
//

#include <errno.h>

#include <boost/bind.hpp>

#include "EventLoop.h"
#include "TcpConnection.h"
#include "SocketOpts.h"
#include "Socket.h"
#include "Channel.h"

#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

TcpConnection::TcpConnection(EventLoop* loop,
				const std::string& name,
				int sockfd,
				const InetAddress& peerAddr,
				const InetAddress& localAddr):
		loop_(loop),
		name_(name),
		socket_(new Socket(sockfd)),
		channel_(new Channel(loop, sockfd)),
		state_(kConnecting),
		peerAddr_(peerAddr),
		localAddr_(localAddr)
{
	channel_->setReadCallback(
					boost::bind(&TcpConnection::handleRead, this, _1));
	channel_->setWriteCallback(
					boost::bind(&TcpConnection::handleWrite, this));
	channel_->setCloseCallback(
					boost::bind(&TcpConnection::handleClose, this));
	channel_->setErrorCallback(
					boost::bind(&TcpConnection::handleError, this));
	LOG_DEBUG<<"TcpConnection::construtor, "<< "name "
			<<name_<<" fd: "<<socket_;
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
	
	loop_->assertInLoopThread();
	int savedErrno;
	int nread = inputBuffer_.readFd(channel_->fd(), &savedErrno);
	if(nread > 0)
	{
		messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
	}
	else if(nread == 0)
	{
		handleClose();
	}else
	{
		//handle savedErrno 
	}
}

void TcpConnection::handleWrite()
{
	loop_->assertInLoopThread();
	if(channel_->isWriting())
	{
		//enable writing
		ssize_t nwrite = ::write(channel_->fd(),
						outputBuffer_.peek(), outputBuffer_.readableBytes());

		if(nwrite > 0)
		{
			outputBuffer_.retrieve(nwrite);
			if(outputBuffer_.readableBytes() == 0)
			{
				//have wrote all the content of buffer
				channel_->disableWriting();
				//save time
				if(writeCompleteCallback_)
					loop_->queueInLoop(boost::bind(writeCompleteCallback_, shared_from_this()));
				if(state_ == kDisconnecting)
				{
					shutdown();
				}
			}
			else
			{
				LOG_TRACE<<"I am going to write more";
			}
		
		}//end nwrite
		else
		{
			LOG_SYSERR<<"TcpConnection::handleWrite";
		}
	}//end channel
	else
	{
		LOG_TRACE<<"TcpConnection::handleWrite !isWriting";
	}
}

void TcpConnection::handleClose()
{
	loop_->assertInLoopThread();
	assert(state_==kConnected);
	//setState(kDisconnected);
	
	//socket destructor will close fd
	channel_->disableAll();
	//must the last line
	closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
	//call getsocketopt()
	int err = sockets::getSocketError(channel_->fd());
	LOG_ERROR<<"TcpConnection::handleError["<<name_
			<<"] - SO_ERROR = "<<err<<" "<<strerror_tl(err);//call strerror_r()
}

void TcpConnection::send(const void* message, size_t len)
{
	if(state_ == kConnected)
	{
		if(loop_->isInLoopThread())
		{
			sendInLoop(message, len);
		}
		else
		{
			std::string data(static_cast<const char*>(message), len);
			loop_->runInLoop(
					boost::bind(&TcpConnection::sendInLoop, this, data));
		}
	}
}

void TcpConnection::send(const std::string& message)
{
	//send(message.data(), message.size());
	if(state_ == kConnected)
	{
		if(loop_->isInLoopThread())
		{
			sendInLoop(message);
		}
		else
		{
			loop_->runInLoop(
					boost::bind(&TcpConnection::sendInLoop, this, message));
		}
	}
}

void TcpConnection::send(Buffer* buffer)
{
	//send(buffer->peek(), buffer->readableBytes());
	if(state_ == kConnected)
	{
		if(loop_->isInLoopThread())
		{
			sendInLoop(buffer->peek(), buffer->readableBytes());
			buffer->retrieveAll();
		}
		else
		{
			loop_->runInLoop(
					boost::bind(&TcpConnection::sendInLoop, 
							this,
							buffer->retrieveAsString()));
		}
	}
}

void TcpConnection::sendInLoop(const std::string& message)
{
	sendInLoop(message.data(), message.size());
}


void TcpConnection::sendInLoop(const void* message, size_t len)
{
	loop_->assertInLoopThread();
	ssize_t nwrite = 0;
	//make sure output buffer is empty
	if(!channel_->isWriting()&&outputBuffer_.readableBytes() == 0)
	{
		nwrite = ::write(channel_->fd(), message, len);
		if(nwrite >= 0)
		{
			if(static_cast<size_t>(nwrite) < len)
			{
				LOG_TRACE<<"I am going to write more";
			}
			else if(writeCompleteCallback_)
			{
				loop_->queueInLoop(
						boost::bind(writeCompleteCallback_, shared_from_this()));
			}
		}
		else//nwrite < 0
		{
			nwrite = 0;
			if(errno != EWOULDBLOCK)
				LOG_SYSERR<<"TcpConnection::sendInLoop";
		}

	}	
	assert(nwrite >= 0);
	if(static_cast<size_t>(nwrite) < len)
	{
		outputBuffer_.append(
				static_cast<const char*>(message) + nwrite, len - nwrite);
		if(!channel_->isWriting())
		{
			channel_->enableWriting();
		}
	}	
}

void TcpConnection::shutdown()
{
	if(state_ == kConnected)
	{
		setState(kDisconnecting);
		loop_->runInLoop(
				boost::bind(&TcpConnection::shutdownInLoop, this));
	}
}

void TcpConnection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	if(!channel_->isWriting())
	{
		socket_ -> shutdownWrite();
	}
}

void TcpConnection::connectionEstablished()
{
	loop_->assertInLoopThread();
	assert(state_ == kConnecting);
	setState(kConnected);
	channel_->enableReading();

	if(connectionCallback_)
		connectionCallback_(shared_from_this());
}

void TcpConnection::connectionDestroyed()
{
	loop_->assertInLoopThread();
	if(state_ == kConnecting)
	{
		setState(kDisconnected);
		channel_->disableAll();

		connectionCallback_(shared_from_this());
	}

	loop_->removeChannel(channel_.get());
}



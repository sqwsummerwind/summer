//2016.9.22
//qiangwei.su
//
#ifndef SUMMER_NET_TCPCONNECTION_H
#define SUMMER_NET_TCPCONNECTION_H

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include "Callback.h"
#include "Buffer.h"
#include "InetAddress.h"

#include "../base/Timestamp.h"

namespace summer
{
namespace net
{
	class EventLoop;
	class Channel;
	class Socket;

	class TcpConnection : public boost::enable_shared_from_this<TcpConnection>,
						  boost::noncopyable 
	{

		public:
			TcpConnection(EventLoop* loop,const std::string& name,
							int sockfd, const InetAddress& peerAddr,
							const InetAddress& localAddr);
			//~TcpConnection();

			EventLoop* getLoop() const
			{
				return loop_;
			}

			const std::string& getName() const
			{
				return name_;
			}

			const InetAddress& getPeerAddr() const
			{
				return peerAddr_;
			}

			const InetAddress& getLocalAddr() const
			{
				return localAddr_;
			}

			bool connected()
			{
				return state_ == kConnected;
			}

			void setConnectionCallback(const ConnectionCallback& cb)
			{
				connectionCallback_ = cb;
			}

			void setCloseCallback(const CloseCallback& cb)
			{
				closeCallback_ = cb;
			}

			void setWriteCompleteCallback(const WriteCompleteCallback& cb)
			{
				writeCompleteCallback_ = cb;
			}

			void setMessageCallback(const MessageCallback& cb)
			{
				messageCallback_ = cb;
			}

			void setContext(const boost::any& context)
			{
				context_ = context;
			}

			boost::any& getContext()
			{
				return context_;
			}

			const boost::any& getContext() const
			{
				return context_;
			}

			void send(const std::string& message);
			void send(const void* buf, size_t len);
			void send(Buffer* buffer);

			void shutdown();

			//call when tcpserver establish a new connection
			void connectionEstablished();
			//call when tcpserver remove me from map
			void connectionDestroyed();
		private:
			
			//when fd read to read;bind to channel's readcallback;call buffer's readfd()
			void handleRead(Timestamp receiveTime);
			//bind to channel's writecallback;
			void handleWrite();
			//maybe call by handleRead();
			void handleClose();
			void handleError();

			void sendInLoop(const std::string& message);
			void sendInLoop(const void* message, size_t len);
			void shutdownInLoop();
			
			//kconnecting -- contructor
			//kconnected -- connectionestablished
			//kdisconnected -- connectionDestroyed
			//kdisconnecting -- shutdown
			enum StateE { kConnected, kConnecting,
					kDisconnected, kDisconnecting};

			StateE state_;
			void setState(StateE state)
			{
				state_ = state;
			}

			EventLoop* loop_;
			std::string name_;
			boost::shared_ptr<Socket> socket_;
			boost::shared_ptr<Channel> channel_;
			InetAddress peerAddr_;
			InetAddress localAddr_;
			ConnectionCallback connectionCallback_;
			WriteCompleteCallback writeCompleteCallback_;
			MessageCallback messageCallback_;
			CloseCallback closeCallback_;
			boost::any context_;

			Buffer inputBuffer_;
			Buffer outputBuffer_;

	};
}
}


#endif



//2016.9.21
//qiangwei.su
//
#ifndef SUMMER_NET_ACCEPTOR_H
#define SUMMER_NET_ACCEPTOR_H

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include "Channel.h"
#include "Socket.h"

namespace summer
{
namespace net
{
	class EventLoop;
	class InetAddress;

	class Acceptor:public boost::noncopyable
	{
		public:
			typedef boost::function<void(int sockfd,
							const InetAddress&)> NewConnectionCallback;
			Acceptor(EventLoop* loop, const InetAddress& listenAddr);
			void setNewConnectionCallback(const NewConnectionCallback& cb)
			{
				newConnectionCallback_ = cb;
			}
			bool listening()
			{
				return listening_;
			}

			void listen();
		private:
			void handleRead();	//accept new connection
			
			EventLoop* loop_;
			Socket acceptSocket_;
			Channel acceptChannel_;
			NewConnectionCallback newConnectionCallback_;
			bool listening_;

	};
}
}

#endif



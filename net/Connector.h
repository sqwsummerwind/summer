//2016.10.11
//qiangwei.su
//
#ifndef SUMMER_NET_CONNECTOR_H
#define SUMMER_NET_CONNECTOR_H

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "Callback.h"

#include "InetAddress.h"

namespace summer
{
namespace net
{
	class Channel;
	class EventLoop;

	class Connector:boost::noncopyable
	{
		public:
			Connector(EventLoop* loop, const InetAddress& serverAddr);
			~Connector();
			void setNewConnectionCallback(const NewConnectionCallback& cb)
			{
				newConnectionCallback_ = cb;
			}
			void start();	//can be called in any thread
			void restart();	//only be called in loop thread
			//void stop();	//can be called in any thread
			const InetAddress& serverAddress() const
			{
				return serverAddr_;
			}
			
		private:
			enum State{ kDisConnected, kConnected, kConnecting};
			static const int kMaxRetryDelayMs = 30*1000;
			static const int kInitRetryDelayMs = 500;

			void setState(State state)
			{
				state_ = state;
			}
			void startInLoop();	//called by start or retry
			void retry(int sockfd);
			void connecting(int sockfd);
			int removeAndResetChannel();	//return sockfd
			void resetChannel();	//call channel_.reset();
			void handleWrite();
			void handleError();

			EventLoop* loop_;
			State state_;
			InetAddress serverAddr_;
			boost::scoped_ptr<Channel> channel_;
			NewConnectionCallback newConnectionCallback_;
			int retryDelayMs_;
	};

}
}



#endif



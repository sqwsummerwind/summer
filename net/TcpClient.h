//2016.10.11
//qiangwei.su
//

#ifndef SUMMER_NET_TCPCLIENT_H
#define SUMMER_NET_TCPCLIENT_H

#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

#include "Callback.h"
#include "InetAddress.h"

#include "../base/Mutex.h"

namespace summer
{
namespace net
{
	class EventLoop;
	class Connector;

	class TcpClient:boost::noncopyable
	{
		public:
			TcpClient(EventLoop* loop,
							const InetAddress& serverAddr, 
							const std::string& name);
			//~TcpClient();
			void connect();
			void disconnect();

			void setMessageCallback(const MessageCallback& cb)
			{
				messageCallback_ = cb;
			}

			void setConnectionCallback(const ConnectionCallback& cb)
			{
				connectionCallback_ = cb;
			}

			void setWriteCompleteCallback(const WriteCompleteCallback& cb)
			{
				writeCompleteCallback_ = cb;
			}

			//bool retry() const;
			void enableRetry()
			{
				retry_ = true;
			}

		private:
			
			void newConnection(int sockfd);
			void removeConnection(const TcpConnectionPtr& conn);

			EventLoop* loop_;
			boost::scoped_ptr<Connector> connector_;
			const std::string name_;
			MessageCallback messageCallback_;
			WriteCompleteCallback writeCompleteCallback_;
			ConnectionCallback connectionCallback_;
			bool connect_;
			bool retry_;
			int nextConnId_;
			MutexLock mutex_;
			TcpConnectionPtr connection_;
	};
}
}


#endif



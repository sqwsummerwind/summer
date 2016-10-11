//2016.9.23
//qiangwei.su
//
#ifndef SUMMER_NET_TCPSERVER_H
#define SUMMER_NET_TCPSERVER_H

#include <map>
#include <string>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "InetAddress.h"
#include "Callback.h"
//#include "EventLoopThreadPool.h"

namespace summer
{
namespace net
{
	class EventLoop;
	class EventLoopThreadPool;
	class Acceptor;

	class TcpServer : public boost::noncopyable
	{
		public:
			TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name);
			//~TcpServer();
			
			void start();

			const std::string& name() const
			{
				return name_;
			}

			const std::string& hostPort() const 
			{
				return hostPort_;
			}

			void setConnectionCallback(const ConnectionCallback& cb)
			{
				connectionCallback_ = cb;
			}

			void setMessageCallback(const MessageCallback& cb)
			{
				messageCallback_ = cb;
			}

			void setWriteComleteCallback(const WriteCompleteCallback& cb)
			{
				writeCompleteCallback_ = cb;
			}

			void setThreadNum(int numThreads);
		private:
			//bind to acceptor's newconnection callback
			void newConnection(int sockfd, const InetAddress& peerAddr);
			void removeConnection(const TcpConnectionPtr& conn);
			void removeConnectionInLoop(const TcpConnectionPtr& conn);

			typedef std::map<std::string, TcpConnectionPtr> TcpConnectionPtrMap;
			
			//loop for acceptor
			EventLoop* loop_;
			const std::string name_;
			const std::string hostPort_;
			boost::scoped_ptr<Acceptor> acceptor_;
			boost::scoped_ptr<EventLoopThreadPool> threadPool_;
			ConnectionCallback connectionCallback_;
			MessageCallback messageCallback_;
			WriteCompleteCallback writeCompleteCallback_;

			bool started_;
			int nextConnId_;
			TcpConnectionPtrMap connections_;
	};
}
}


#endif



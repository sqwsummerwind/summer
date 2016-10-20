//2016.10.11
//qiangwei.su
//

#include <string>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <mcheck.h>

#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>

#include "TcpServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Callback.h"
#include "Acceptor.h"
#include "EventLoopThreadPool.h"

#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

int ThreadNum = 0;

class EchoServer:boost::noncopyable
{
	public:
		EchoServer(EventLoop* loop, const InetAddress& addr):
		loop_(loop),
		server_(loop, addr, "EchoServer")
		//server_(new TcpServer(loop, addr, name))
		{
			server_.setConnectionCallback(
					boost::bind(&EchoServer::onConnection, this, _1));
			server_.setMessageCallback(
					boost::bind(&EchoServer::onMessage, this, _1, _2, _3));
			server_.setThreadNum(ThreadNum);
		}

		void start()
		{
			//server_->start();
			server_.start();
		}

	private:

		void onConnection(const TcpConnectionPtr& conn)
		{
			LOG_TRACE<<conn->getPeerAddr().toHostPort()<<" -> "<<
					conn->getLocalAddr().toHostPort()<< " is "<<
					(conn->connected() ? "UP" : "DOWN");
			conn->send("Hello\n");
		}

		void onMessage(const TcpConnectionPtr& conn, 
						Buffer* buf, Timestamp timestamp)
		{
			std::string msg(buf->retrieveAsString());
			LOG_TRACE<<conn->getName()<<" receive "<<
					msg << " at "<<timestamp.toString();
			if(msg == "exit\n")
			{
				conn->send("bye\n");
				conn->shutdown();
			}

			if(msg == "quit\n")
			{
				loop_->quit();
			}
			conn->send(msg);
		}

		EventLoop* loop_;
		//boost::scoped_ptr<TcpServer> server_;
		TcpServer server_;
};

int main(int argc, char** argv)
{
	mtrace();
	LOG_INFO<<"pid = "<<getpid()<<" tid = "<<CurrentThread::tid();
	LOG_INFO<<"size of TcpConnection: "<<sizeof(TcpConnection);

	if(argc > 1)
	{
		ThreadNum = atoi(argv[1]);
	}

	EventLoop loop;
	InetAddress listenAddr(2016);
	LOG_DEBUG<<"listenAddr :"<<listenAddr.toHostPort();
	EchoServer echoServer(&loop, listenAddr);
	
	echoServer.start();
	
	loop.loop();
}

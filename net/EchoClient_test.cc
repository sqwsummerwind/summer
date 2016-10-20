//2016.10.11
//qiangwei.su
//

#include <string>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include "TcpClient.h"
#include "EventLoop.h"
#include "InetAddress.h"
#include "Callback.h"
#include "Buffer.h"
#include "TcpConnection.h"
#include "Connector.h"

#include "../base/Timestamp.h"
#include "../base/Logging.h"

using namespace summer;
using namespace summer::net;

class EchoClient;
int current = 0;
//int ClientNum;
std::vector< boost::shared_ptr<EchoClient> > clients_;
//std::vector<EchoClient*> clients_;

class EchoClient
{
	public:
		EchoClient(EventLoop* loop, const InetAddress& peerAddr, std::string& name):
		loop_(loop),
		client_(loop, peerAddr, name)
		{
			client_.setMessageCallback(
					boost::bind(&EchoClient::onMessage, this, _1, _2, _3));
			client_.setConnectionCallback(
					boost::bind(&EchoClient::onConnection, this, _1));
		}

		void connect()
		{
			client_.connect();
		}

	private:

		void onConnection(const TcpConnectionPtr& conn)
		{
			LOG_TRACE<<conn->getLocalAddr().toHostPort()<<" -> "
					<<conn->getPeerAddr().toHostPort()<<" is "
					<<(conn->connected() ? "UP":"DOWN");
			
			++current;
			if(static_cast<size_t>(current) < clients_.size())
			{
				clients_[current]->connect();

				LOG_TRACE<<"*** connected";

				conn->send("World\n");
			}
		}

		void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
		{
			std::string msg(buf->retrieveAsString());
			LOG_TRACE<<conn->getName()<<" receive "<<msg<<" at "<<time.toString();
		}

		EventLoop* loop_;
		TcpClient client_;
};


int main(int argc, char* argv[])
{
	LOG_TRACE<<"pid = "<<getpid()<<" tid = "<<CurrentThread::tid();
	if(argc<2)
	{
		printf("usage:%s host_ip [current#]", argv[0]);
		return 0;
	}

	int n = 1;
	if(argc > 2)
	{
		n = atoi(argv[2]);
	}
	clients_.reserve(n);

	EventLoop loop;
	InetAddress peerAddr(argv[1], 2016);

	for(int i = 0; i < n; i++)
	{
		char buf[32];
		snprintf(buf, sizeof buf, "%s%d","EchoClient", i+1);
		std::string name(buf);
		clients_.push_back(boost::shared_ptr<EchoClient>(new EchoClient(&loop, peerAddr, name)));
		//clients_[i].push_back(new EchoClient(&loop, peerAddr, buf));
	}

	clients_[current]->connect();
	loop.loop();

	return 0;
}

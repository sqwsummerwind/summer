//2016.10.18
//qiangwei.su
//

#include <boost/bind.hpp>
#include <boost/any.hpp>

#include <string>
#include <set>
#include <map>
#include <stdio.h>
#include <stdlib.h>

#include "../Callback.h"
#include "../TcpServer.h"
#include "../EventLoop.h"
#include "../TcpConnection.h"
#include "../EventLoopThreadPool.h"
#include "../Acceptor.h"
#include "../../base/Timestamp.h"
#include "../../base/Logging.h"

#include "codec.h"

using namespace summer;
using namespace summer::net;
using namespace pubsub;

class Topic
{
	public:
		Topic(std::string topic):
		topic_(topic)
		//audiences_(std::set<TcpConnectionPtr>())
		{
			LOG_TRACE<<"audiences_ size: "<<audiences_.size();
		}

		void add(const TcpConnectionPtr& conn)
		{
			LOG_TRACE<<" conn name:"<<conn->getName();
			LOG_TRACE<<" audiences_ size:"<<audiences_.size();
			audiences_.insert(conn);
			if(lastPubTime_.valid())
			{
				std::string message = makeMessage();
				conn->send(message);
			}
			LOG_TRACE<<" end";
		}

		void remove(const TcpConnectionPtr conn)
		{
			audiences_.erase(conn);
		}

		void publish(const std::string& content, Timestamp timestamp)
		{
			content_ = content;
			lastPubTime_= timestamp;
			std::string message = makeMessage();
			for(std::set<TcpConnectionPtr>::iterator begin = audiences_.begin();
							begin != audiences_.end(); ++begin)
			{
				(*begin)->send(message);
			}
		}


	private:

		std::string makeMessage()
		{
			return "pub "+topic_+"\r\n"+content_+"\r\n";
		}

		std::string topic_;
		Timestamp lastPubTime_;
		std::string content_;
		//typedef std::set<TcpConnectionPtr> TcpConnSet;
		std::set<TcpConnectionPtr> audiences_;
};

class HubServer
{
	public:
		HubServer(EventLoop* loop, const InetAddress& addr):
		loop_(loop),
		server_(loop, addr, "HubServer")
		{
			server_.setConnectionCallback(
							boost::bind(&HubServer::onConnection, this, _1));
			server_.setMessageCallback(
							boost::bind(&HubServer::onMessage, this, _1, _2, _3));
		}

		void start()
		{
			server_.start();
		}

	private:

		Topic& getTopic(const std::string& topic)
		{
			LOG_TRACE<<"topic: "<<topic;
			MapTopic::iterator it = topics_.find(topic);
			//topic not exist, insert
			if(it == topics_.end())
			{
				it = topics_.insert(std::make_pair(topic, Topic(topic))).first;
			}

			return it->second;
		}

		void onConnection(const TcpConnectionPtr& conn)
		{
			//the connection establish
			LOG_TRACE<<"onConnection";
			if(conn->connected())
			{
				conn->setContext(std::set<std::string>());
			}else{
			//the connection destroy
				std::set<std::string> topics = boost::any_cast< std::set<std::string> >(conn->getContext());
				for(std::set<std::string>::iterator it = topics.begin();
								it != topics.end(); ++it)
				{
					doUnSubscribe(conn, *it);
				}
			}	
		}

		void onMessage(const TcpConnectionPtr& conn,
						Buffer* message,
						Timestamp timestamp)
		{
			std::string topic;
			std::string cmd;
			std::string content;
			ParseResult parseResult = kSuccess;
			while(parseResult == kSuccess)
			{
				parseResult	= parseMessage(message, &cmd, &topic, &content);
				
				if(parseResult == kSuccess)
				{
				
					if(cmd == "pub")
					{
						doPublish(topic, content, Timestamp::now());
					}else if(cmd == "sub")
					{
						LOG_TRACE<<"sub "<< topic;
						doSubscribe(conn, topic);
					}else if(cmd == "unsub")
					{
						doUnSubscribe(conn, topic);
					}
				}
			}//end kSuccess
		}

		void doUnSubscribe(const TcpConnectionPtr& conn, const std::string& topic)
		{

			std::set<std::string> topics = boost::any_cast<std::set<std::string> >(conn->getContext());
			topics.erase(topic);
			getTopic(topic).remove(conn);
		}

		void doSubscribe(const TcpConnectionPtr& conn, const std::string& topic)
		{
			LOG_TRACE<<"do subscribe topic: "<<topic;
			std::set<std::string> topics = boost::any_cast<std::set<std::string> >(conn->getContext());
			topics.insert(topic);
			getTopic(topic).add(conn);
		}

		void doPublish(const std::string& topic, const std::string& content, Timestamp timestamp)
		{
			getTopic(topic).publish(content, timestamp);
		}

		EventLoop* loop_;
		TcpServer server_;
		typedef std::map<std::string, Topic> MapTopic;
		MapTopic topics_;
};



int main(int argc, char* argv[])
{
	if(argc > 1)
	{
		EventLoop loop;
		uint16_t port =  static_cast<uint16_t>(atoi(argv[1]));
		InetAddress addr(port);
		HubServer hubServer(&loop, addr);
		hubServer.start();
		loop.loop();
	}else
	{
		printf("usage:%s port\n", argv[0]);
	}
}

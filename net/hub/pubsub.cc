//2016.10.19
//qiangwei.su
//

#include <boost/bind.hpp>

#include "../EventLoop.h"
#include "../InetAddress.h"
#include "../TcpConnection.h"
#include "../Connector.h"

#include "pubsub.h"
#include "codec.h"

using namespace pubsub;
using namespace summer;
using namespace summer::net;

PubSubClient::PubSubClient(EventLoop* loop,
				const InetAddress& peerAddr,
				const std::string& name):
loop_(loop),
client_(loop, peerAddr, name)
{
	client_.setConnectionCallback(
					boost::bind(&PubSubClient::onConnection, this, _1));
	client_.setMessageCallback(
					boost::bind(&PubSubClient::onMessage, this, _1, _2, _3));
}

void PubSubClient::connect()
{
	client_.connect();
}

void PubSubClient::disconnect()
{
	client_.disconnect();
}

void PubSubClient::onConnection(const TcpConnectionPtr& conn)
{
	if(conn->connected())
	{
		conn_ = conn;
	}else
	{
		conn_.reset();
	}

	if(connectionCallback_)
	{
		connectionCallback_(this);
	}
}

void PubSubClient::onMessage(const TcpConnectionPtr& conn, 
				Buffer* buf, 
				Timestamp time)
{
	ParseResult parseResult = kSuccess;
	while(parseResult == kSuccess)
	{
		std::string cmd;
		std::string topic;
		std::string content;
		parseResult = parseMessage(buf, &cmd, &topic, &content);
		if(parseResult == kSuccess)
		{
			if(cmd == "pub" && subscribeCallback_)
			{
				subscribeCallback_(topic, content, time);
			}
		}else if(parseResult == kError)
		{
			conn->shutdown();
		}
	}
}

bool PubSubClient::connected() const
{
	return conn_&&conn_->connected();
}

void PubSubClient::subscribe(const std::string& topic)
{
	std::string message = "sub "+ topic + "\r\n";
	send(message);

}

void PubSubClient::unSubscribe(const std::string& topic)
{
	std::string message = "unsub "+ topic + "\r\n";
	send(message);
}

void PubSubClient::publish(const std::string& topic, const std::string& content)
{
	std::string message = "pub " + topic + "\r\n" + content + "\r\n";
	send(message);
}

bool PubSubClient::send(const std::string& message)
{
	bool result = false;
	if(connected())
	{
		result = true;
		conn_->send(message);
	}

	return result;
}

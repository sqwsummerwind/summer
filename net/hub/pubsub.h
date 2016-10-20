//2016.10.19
//qiangwei.su
//

#ifndef SUMMER_NET_HUB_PUBSUB_H
#define SUMMER_NET_HUB_PUBSUB_H

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

#include "../TcpClient.h"
#include "../Callback.h"
#include "../Buffer.h"
#include "../../base/Timestamp.h"

namespace summer
{
namespace net
{
	class EventLoop;
	class InetAddress;

	class PubSubClient:public boost::noncopyable
	{
		public:
			typedef boost::function<void (PubSubClient*)> ConnectionCallback;
			typedef boost::function<void (std::string& topic,
							std::string& content,
							Timestamp receiveTime)> SubScribeCallback;
			PubSubClient(EventLoop* loop,
							const InetAddress& peerAddr,
							const std::string& name);
			void connect();
			void disconnect();
			void subscribe(const std::string& topic);
			void unSubscribe(const std::string& topic);
			void publish(const std::string& topic, const std::string& content);
			void setConnectionCallback(const ConnectionCallback& cb)
			{
				connectionCallback_ = cb;
			}

			void setSubscribeCallback(const SubScribeCallback& cb)
			{
				subscribeCallback_ = cb;
			}

			bool connected() const;

		private:
			void onConnection(const TcpConnectionPtr& conn);
			void onMessage(const TcpConnectionPtr& conn, 
							Buffer* buf, 
							Timestamp time);
			bool send(const std::string& message);
			EventLoop* loop_;
			TcpClient client_;
			ConnectionCallback connectionCallback_;
			SubScribeCallback subscribeCallback_;
			TcpConnectionPtr conn_;
	};
}
}

#endif



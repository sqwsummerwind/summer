//2016.10.19
//qiangwei.su
//
//

#include <vector>
#include <string>
#include <stdio.h>

#include "pubsub.h"

#include "../EventLoop.h"
#include "../Connector.h"

using namespace summer;
using namespace summer::net;

std::vector<std::string> g_topics;

void connection(PubSubClient* client)
{
	if(client->connected())
	{
		for(int i = 0; i < g_topics.size(); ++i)
		{
			client->subscribe(g_topics[i]);
		}
	}
}

void message(const std::string& topic,
				const std::string& content,
				Timestamp receiveTime)
{
	printf("%s: %s\n", topic.c_str(), content.c_str());
}

int main(int argc, char* argv[])
{
	if(argc > 2)
	{
		std::string hostport = argv[1];
		size_t pos = hostport.find(':');
		//find
		if(pos != std::string::npos)
		{
			std::string ip = hostport.substr(0, pos);
			uint16_t port = static_cast<uint16_t>(atoi(hostport.c_str() + pos +1));
			for(int i = 2; i < argc; ++i)
			{
				g_topics.push_back(std::string(argv[i]));
			}
			EventLoop loop;
			InetAddress peerAddr(ip, port);
			PubSubClient subClient(&loop, peerAddr, "subClient");
			subClient.setConnectionCallback(connection);
			subClient.setSubscribeCallback(message);
			subClient.connect();
			loop.loop();
		}else{
			//not find
			printf("usage:%s ip:port topic1 topic2 ...\n", argv[0]);
		}
	}else{
		printf("usage:%s ip:port topic1 topic2 ...\n", argv[0]);
	}//end argc
}



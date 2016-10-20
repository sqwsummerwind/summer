//2016.10.19
//qiangwei.su
//

#include <stdio.h>
#include <iostream>

#include "pubsub.h"

#include "../EventLoop.h"
#include "../EventLoopThread.h"
#include "../Connector.h"

using namespace summer;
using namespace summer::net;

EventLoop* g_loop = NULL;
std::string g_topic;
std::string g_content;

void connection(PubSubClient* client)
{
	if(client->connected())
	{
		client->publish(g_topic, g_content);
		client->disconnect();
	}else{
		g_loop->quit();
	}
}


int main(int argc, char* argv[])
{
	if(argc == 4)
	{
		std::string hostport = argv[1];
		std::size_t pos = hostport.find(':');
		//find
		if(pos!=std::string::npos)
		{
			std::string ip = hostport.substr(0, pos);
			uint16_t port = static_cast<uint16_t>(atoi(hostport.c_str()+pos+1));
			g_topic = argv[2];
			g_content = argv[3];
			//using stdin for content input
			if(g_content == "-")
			{
				InetAddress peerAddr(ip, port);
				EventLoopThread eventLoop;
				PubSubClient pubClient(eventLoop.startLoop(), peerAddr, "pubClient");
				pubClient.connect();
				std::string line;
				while(std::getline(std::cin, line))
				{
					pubClient.publish(g_topic, line);
				}
				pubClient.disconnect();
			}else{
				EventLoop loop;
				g_loop = &loop;
				InetAddress peerAddr(ip, port);
				PubSubClient pubClient(g_loop, peerAddr, "pubClient");
				pubClient.setConnectionCallback(connection);
				pubClient.connect();
				loop.loop();
			}//end g_content
		}else{

			printf("usage: %s ip:port topic content", argv[0]);
		}//end pos

	}
	else
	{
		printf("usage:%s ip:port topic - \n", argv[0]);
		printf("usage:%s ip:port topic content \n", argv[0]);
	}//end argc
}



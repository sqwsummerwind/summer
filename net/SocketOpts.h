//2016.9.21
//qiangwei.su

#ifndef SUMMER_NET_SOCKETOPTS_H
#define SUMMER_NET_SOCKETOPTS_H
#include <arpa/inet.h>


namespace summer
{
	namespace net
	{
		namespace sockets
		{
			inline uint32_t hostToNetwork32(uint32_t hostlong)
			{
				return htonl(hostlong);
			}

			inline uint32_t networkToHost32(uint32_t netlong)
			{
				return ntohl(netlong);
			}

			inline uint16_t hostToNetwork16(uint16_t hostshort)
			{
				return htons(hostshort);
			}

			inline uint16_t networkToHost16(uint16_t netshort)
			{
				return ntohs(netshort);
			}

			int createNonblockingOrDie();

			int connect(int sockfd, const struct sockaddr_in& addr);
			void bindOrDie(int sockfd, const struct sockaddr_in& addr);
			void listenOrDie(int sockfd);
			int accept(int sockfd, struct sockaddr_in* addr);
			void close(int sockfd);
			void shutdownWrite(int sockfd);

			void toHostPort(char* buf, size_t size,
							const struct sockaddr_in& addr);
			void fromHostPort(const char* ip, uint16_t port,
							struct sockaddr_in* addr);

			int getSocketError(int sockfd);

			struct sockaddr_in getLocalAddr(int sockfd);
			struct sockaddr_in getPeerAddr(int sockfd);
			bool isSelfConnect(int sockfd);

		}
	
	}

}



#endif



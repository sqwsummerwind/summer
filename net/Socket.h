//2016.9.21
//qiangwei.su

#ifndef SUMMER_NET_SOCKET_H
#define SUMMER_NET_SOCKET_H

#include <boost/noncopyable.hpp>


namespace summer
{
namespace net
{
	class InetAddress;

	class Socket: boost::noncopyable
	{
		public:
			Socket(int sockfd):
			sockfd_(sockfd)
			{}

			~Socket();

			void bindAddress(const InetAddress& listenAddr);
			void listen();

			int accept(InetAddress* peerAddr);
			//int connect(const InetAddress& addr);
			
			int fd()
			{
				return sockfd_;
			}

			void setTcpNoDelay(bool on);

			void setReuseAddr(bool on);

			void shutdownWrite();

		private:
			const int sockfd_;
			

	};
}
}

#endif



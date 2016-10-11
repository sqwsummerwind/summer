//2016.9.21
//qiangwei.su


#ifndef SUMMER_NET_INETADDRESS_H
#define SUMMER_NET_INETADDRESS_H

#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdint.h>


namespace summer
{
namespace net
{
	class InetAddress
	{
		public:
			InetAddress(uint16_t port);

			InetAddress(const std::string& ip, uint16_t port);

			InetAddress(const struct sockaddr_in& addr):
			addr_(addr)
			{}

			struct sockaddr_in getInetAddr() const
			{
				return addr_;
			}

			void setInetAddr(const struct sockaddr_in& addr)
			{
				addr_ = addr;
			}

			std::string toHostPort() const;

		private:
			struct sockaddr_in addr_;
	};

}
}

#endif



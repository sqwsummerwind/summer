//2016.9,22
//qiangwei.su
//
#ifndef SUMMER_NET_CALLBACK_H
#define SUMMER_NET_CALLBACK_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include "../base/Timestamp.h"

namespace summer
{
namespace net
{	
	class Buffer;
	class TcpConnection;

	typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;
	typedef boost::function<void (const TcpConnectionPtr&)> CloseCallback;
	typedef boost::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
	typedef boost::function<void (const TcpConnectionPtr&, 
					Buffer*,
					Timestamp timestamp)> MessageCallback;
}
}

#endif



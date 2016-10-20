//2016.10.18
//qiangwei.su
//
#ifndef SUMMER_NET_HUB_CODEC_H
#define SUMMER_NET_HUB_CODEC_H

#include <string>

#include "../Buffer.h"

namespace pubsub
{
	enum ParseResult{ kSuccess, kContinue, kError};
	ParseResult parseMessage(summer::net::Buffer* message,
					std::string* cmd,
					std::string* topic,
					std::string* content);

}
#endif

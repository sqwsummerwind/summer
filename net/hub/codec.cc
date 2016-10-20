//2016.10.18
//qiangwei.su
//

#include "codec.h"

using namespace summer;
using namespace summer::net;
//using namespace pubsub;

//pub topic \r\n content \r\n
//sub topic \r\n
//unsub topic \r\n
pubsub::ParseResult pubsub::parseMessage(Buffer* message,
				std::string* cmd,
				std::string* topic,
				std::string* content)
{
	const char* crlf = message->findCRLF();
	//ParseResult state = kSuccess;
	if(crlf)
	{
		const char* spacePos = std::find(message->peek(), crlf, ' ');
		if(spacePos!=crlf)
		{
			cmd->assign(message->peek(), spacePos);
			topic->assign(spacePos + 1, crlf);
			if(*cmd == "pub")
			{
				const char* secCrlf = message->findCRLF(crlf+2);
				if(crlf)
				{
					content->assign(crlf + 2, secCrlf);
					message->retrieve(secCrlf+2);
					return kSuccess;
				}else{
					return kContinue;
				}
			}//end pub
			else{
				message->retrieve(crlf+2);
				return kSuccess;
			}
		}//end second crlf
		else
		{
			return kError;
		}
	}//end first crlf
	else
	{
		return kContinue;
	}
}

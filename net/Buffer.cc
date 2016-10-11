//2016.9.21
//qiangwei.su
//

#include <errno.h>
#include <sys/uio.h>

#include <boost/implicit_cast.hpp>

#include "Buffer.h"

using namespace summer;
using namespace summer::net;

const char Buffer::kCRLF[] = "\r\n";

ssize_t Buffer::readFd(int fd, int* savedErrno)
{
	char extrabuf[64 * 1024];
	
	struct iovec iov[2];
	iov[0].iov_base = begin() + writeIndex_;
	size_t writeable = writeableBytes();
	iov[0].iov_len = writeable;
	iov[1].iov_base = extrabuf;
	iov[1].iov_len = sizeof(extrabuf);
	
	ssize_t nread = readv(fd, iov, 2);
	if(nread < 0)
	{
		*savedErrno = errno;
	}
	else if(boost::implicit_cast<size_t>(nread) <= writeable)
	{
		writeIndex_ += nread;
	}
	else
	{
		writeIndex_ = buffer_.size();
		append(extrabuf, nread -writeable);
	}

	return nread;
}


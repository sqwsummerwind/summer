//2016.9.21
//qiangwei.su
//
#ifndef SUMMER_NET_BUFFER_H
#define SUMMER_NET_BUFFER_H

#include <assert.h>
#include <vector>
#include <algorithm>
#include <string>

namespace summer
{
namespace net
{
	class Buffer
	{
		public:
			static const size_t kCheapPrepend = 8;
			static const size_t kInitialSize = 1024;
			
			Buffer():
			buffer_(kCheapPrepend + kInitialSize),
			readIndex_(kCheapPrepend),
			writeIndex_(kCheapPrepend)
			{}

			void swap(Buffer& rbf)
			{
				buffer_.swap(rbf.buffer_);
				std::swap(readIndex_, rbf.readIndex_);
				std::swap(writeIndex_, rbf.writeIndex_);
			}

			size_t readableBytes() const
			{
				return writeIndex_ - readIndex_;
			}

			size_t writeableBytes() const
			{
				return buffer_.size() - writeIndex_;
			}

			size_t prependableBytes() const
			{
				return readIndex_;
			}

			const char* peek() const
			{
				return begin() + readIndex_;
			}

			const char* findCRLF() const
			{
				const char* crlf = std::search(peek(), writeBegin(), kCRLF, kCRLF + 2);
				return crlf == writeBegin() ? NULL : crlf;
			}

			const char* findCRLF(const char* start) const
			{
				assert(start >= peek());
				assert(start <= writeBegin());
				const char* crlf = std::search(start, writeBegin(), kCRLF, kCRLF + 2);
				return crlf == writeBegin() ? NULL : crlf;
			}

			void retrieve(size_t len)
			{
				assert(len <= readableBytes());
				readIndex_ += len;
			}

			void retrieve(const char* end)
			{
				assert(end >= peek());
				assert(end <= writeBegin());
				readIndex_ += end - peek();
			}
	
			void retrieveAll()
			{
				readIndex_ = kCheapPrepend;
				writeIndex_ = kCheapPrepend;
			}

			std::string retrieveAsString()
			{
				std::string str(peek(), readableBytes());
				readIndex_ = kCheapPrepend;
				writeIndex_ = kCheapPrepend;
				return str;
			}

			void append(const char* data, size_t len)
			{
				if(len > writeableBytes())
				{
					makespace(len);
				}

				assert(len < writeableBytes());

				std::copy(data, data + len, writeBegin());
				writeIndex_ += len;
			}

			void append(const std::string& str)
			{
				append(str.data(), str.length());
			}

			void prepend(const void* data, size_t len)
			{
				assert(len < prependableBytes());
				readIndex_ -= len;
				const char* buf = static_cast<const char*>(data);
				std::copy(buf, buf + len, begin() + readIndex_);
			}

			void shrink(size_t reserve)
			{
				std::vector<char> buf(kCheapPrepend + readableBytes() + reserve);
				size_t used = readableBytes();
				std::copy(begin() + readIndex_,
								begin() + writeIndex_,
								buf.begin() + kCheapPrepend);
				buf.swap(buffer_);
				readIndex_ = kCheapPrepend;
				writeIndex_ = readIndex_+used;
			}

			ssize_t readFd(int fd, int* savedErrno);

		private:

			char* begin()
			{
				return &*buffer_.begin();
			}

			const char* begin() const
			{
				return &*buffer_.begin();
			}

			char* writeBegin()
			{
				return begin()+writeIndex_;
			}

			const char* writeBegin() const
			{
				return begin()+writeIndex_;
			}

			void makespace(size_t more)
			{
				if(writeableBytes() + readIndex_ < more + kCheapPrepend)
				{
					buffer_.resize(writeableBytes() + more);
				}
				else
				{
				size_t used = readableBytes();
				std::copy(begin() + readIndex_,
								begin() + writeIndex_,
								begin() + kCheapPrepend);
				readIndex_ = kCheapPrepend;
				writeIndex_ = kCheapPrepend + used;
				assert(used == readableBytes());
				}
			}


			std::vector<char> buffer_;
			int readIndex_;
			int writeIndex_;
			static const char kCRLF[];
	};
}
}

#endif

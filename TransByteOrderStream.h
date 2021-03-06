#ifndef __TRANS_BYTE_ORDER_STREAM_H_
#define __TRANS_BYTE_ORDER_STREAM_H_

#include <vector>
#include <cassert>


/**
** 源文件： TransByteOrderStream.h TransByteOrderStream.c
** 功能说明： 用于传输的字节流编解码，区分字节的网络序与主机序。
** 类型TransByteOrderIOStream的详细功能已参加代码。
** 开发+测试环境：VS2003 & CodeBlocks10.02
** 作者：junkun huang
** 日期：2010-12-02 /
*/

namespace xl
{
	//////////////////////////////////////////////////////////////////////////

	typedef unsigned char		uint8;
	typedef unsigned short		uint16;
	typedef unsigned int		uint32;
	typedef unsigned __int64	uint64;

	uint8  hton(uint8 n);
	uint16 hton(uint16 n);
	uint32 hton(uint32 n);
	uint64 hton(uint64 n);

	uint8  ntoh(uint8 n);
	uint16 ntoh(uint16 n);
	uint32 ntoh(uint32 n);
	uint64 ntoh(uint64 n);
	bool TestBigEndian();
	bool TestLittleEndian();

}///namespace xl


//////////////////////////////////////////////////////////////////////////
	typedef xl::uint8		backagent_cmd_byte;
	typedef xl::uint16		backagent_cmd_short;
	typedef xl::uint32		backagent_cmd_int;
	typedef xl::uint64		backagent_cmd_long;
	typedef std::string		backagent_cmd_string;

	inline unsigned int CMD_SIZE( backagent_cmd_byte )
	{
		return sizeof(backagent_cmd_byte);
	}

	inline unsigned int CMD_SIZE( backagent_cmd_short )
	{
		return sizeof(backagent_cmd_short);
	}

	inline unsigned int CMD_SIZE( backagent_cmd_int )
	{
		return sizeof(backagent_cmd_int);
	}

	inline unsigned int CMD_SIZE( backagent_cmd_long )
	{
		return sizeof(backagent_cmd_long);
	}

	inline unsigned int CMD_SIZE( const backagent_cmd_string & s )
	{
		return sizeof(backagent_cmd_int)+s.length();
	}

	class TransByteOrderIOStream
	{
	public:
		TransByteOrderIOStream(char * buf_ptr, xl::uint32 buf_len);
		virtual ~TransByteOrderIOStream();

		TransByteOrderIOStream & operator<<(const backagent_cmd_byte & byte_value);
		TransByteOrderIOStream & operator>>(backagent_cmd_byte & byte_value);

		TransByteOrderIOStream & operator<<(const backagent_cmd_short & short_value);
		TransByteOrderIOStream & operator>>(backagent_cmd_short & short_value);

		TransByteOrderIOStream & operator<<(const backagent_cmd_int & int_value);
		TransByteOrderIOStream & operator>>(backagent_cmd_int & int_value);

		TransByteOrderIOStream & operator<<(const backagent_cmd_long & int_value);
		TransByteOrderIOStream & operator>>(backagent_cmd_long & int_value);

		TransByteOrderIOStream & operator<<(const backagent_cmd_string & string_value);
		TransByteOrderIOStream & operator>>(backagent_cmd_string & string_value);

		template <typename T>
			TransByteOrderIOStream& operator<<(const std::vector<T>& vec);
		template <typename T>
			TransByteOrderIOStream& operator>>(std::vector<T>& vec);

		void copy_byte_to(char *byte_array, xl::uint32 len);
		void copy_byte_from(char *byte_array, xl::uint32 len);
		void cross_length(xl::uint32 len);

		char * get_ptr();
		xl::uint32 get_len();
		xl::uint32 get_bytes_read();
		xl::uint32 get_bytes_written();
		void reset_bytes_read();
		void reset_bytes_written();
		void reset();

	private:
		char * _buf_ptr;
		xl::uint32 _buf_len;
		xl::uint32 _buf_size;
		xl::uint32 _bytes_read;
		xl::uint32 _bytes_written;
		template <typename T>
		inline TransByteOrderIOStream& stream_read_int(T int_value);
		template <typename T>
		inline TransByteOrderIOStream& stream_write_to_int(T& int_value);

	};

	template <typename T>
		TransByteOrderIOStream& TransByteOrderIOStream::operator<<(const std::vector<T>& vec )
	{
		*this << static_cast<xl::uint32>(vec.size());
		for (size_t i=0; i < vec.size(); i++)
		{
			*this << vec[i];
		}
		return *this;
	}

	template <typename T>
		TransByteOrderIOStream& TransByteOrderIOStream::operator>>(std::vector<T>& vec )
	{
		xl::uint32 len = 0;
		*this >> len;
		for (size_t i=0; i < len; i++)
		{
			T tmp;
			*this >> tmp;
			vec.push_back(tmp);
		}
		return *this;
	}


//////////////////////////////////////////////////////////////////////////
#endif //__TRANS_BYTE_ORDER_STREAM_H_

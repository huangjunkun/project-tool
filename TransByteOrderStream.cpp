#include "stdafx.h"

#include "./TransByteOrderStream.h"



namespace xl
{
	//////////////////////////////////////////////////////////////////////////

	bool _TestBigEndian()
	{
		union UnionForTest
		{
			short _short;
			char  _char;
		};
		UnionForTest test;
		test._short = 0x0100;
		return 1 == (int)test._char;
	}
	bool TestBigEndian()
	{
		static bool s_is_big_endian = _TestBigEndian();
		return s_is_big_endian;
	}
	bool _TestLittleEndian()
	{
		union UnionForTest
		{
			short _short;
			char  _char;
		};
		UnionForTest test;
		test._short = 0x0001;
		return 1 == (int)test._char;
	}
	bool TestLittleEndian()
	{
		static bool s_is_little_endian = _TestLittleEndian();
		return s_is_little_endian;
	}

	uint16 htons(uint16 n)
	{
		if (TestBigEndian())
			return n;
		return	((n & 0xff) << 8) |
			((n & 0xff00) >> 8);		
	}
	uint16 ntohs(uint16 n) { return htons(n); }

	uint32 htonl(uint32 n)
	{
		if (TestBigEndian())
			return n;
		return ((n & 0xff) << 24) |
			((n & 0xff00) << 8) |
			((n & 0xff0000) >> 8) |
			((n & 0xff000000) >> 24);
	}
	uint32 ntohl(uint32 n) { return htonl(n); }

	uint64 htonll(uint64 n)
	{
		if (TestBigEndian())
			return n;
		release_assert(TestLittleEndian());

		return ((n & 0xff) << 56) |
			((n & 0xff00) << 40) |
			((n & 0xff0000) << 24) |
			((n & 0xff000000) << 8) |
			((n & 0xff00000000) >> 8) |
			((n & 0xff0000000000) >> 24) |
			((n & 0xff000000000000) >> 40) |
			((n & 0xff00000000000000) >> 56);
	}
	uint64 ntohll(uint64 n) { return htonll(n); }

	uint8  hton(uint8 n) { return n; }
	uint16 hton(uint16 n) { return htons(n); }
	uint32 hton(uint32 n) { return htonl(n); }
	uint64 hton(uint64 n) { return htonll(n); }

	uint8  ntoh(uint8 n) { return n; }
	uint16 ntoh(uint16 n) { return ntohs(n); }
	uint32 ntoh(uint32 n) { return ntohl(n); }
	uint64 ntoh(uint64 n) { return ntohll(n); }

	// 	template <typename T>
	// 	T hton (T n)
	// 	{
	// 		if (1 == sizeof(n))
	// 			return n;
	// 		else if (2 == sizeof(n))
	// 			return htons(n);
	// 		else if (4 == sizeof(n))
	// 			return htonl(n);
	// 		else if (8 == sizeof(n))
	// 			return htonll(n);
	// 		else
	// 			release_assert(false);
	// 	}
	// 
	// 	template <typename T>
	// 	T ntoh (T n)
	// 	{
	// 		if (1 == sizeof(n))
	// 			return n;
	// 		else if (2 == sizeof(n))
	// 			return ntohs(n);
	// 		else if (4 == sizeof(n))
	// 			return ntohl(n);
	// 		else if (8 == sizeof(n))
	// 			return ntohll(n);
	// 		else
	// 			release_assert(false);
	// 	}

}///namespace xl


	//////////////////////////////////////////////////////////////////////////

	TransByteOrderIOStream::TransByteOrderIOStream(char * buf_ptr, xl::uint32 buf_len)
		: _buf_ptr(buf_ptr), _buf_len(buf_len),_buf_size(buf_len), _bytes_read(0), _bytes_written(0)
	{
	}

	TransByteOrderIOStream::~TransByteOrderIOStream()
	{
	}


	template <typename T>
	inline TransByteOrderIOStream& TransByteOrderIOStream::stream_read_int(T int_value)
	{
		const xl::uint32 len = sizeof(int_value);
		if ( _buf_len < len )
		{
			ATLASSERT(FALSE);
			return *this;
		}	
		T trans_value = xl::hton(int_value);//主机序转为网络序
		memcpy( _buf_ptr, &trans_value, len );
		_buf_ptr += len;
		_buf_len -= len;
		_bytes_written += len;
		return *this;
	}

	template <typename T>
	inline TransByteOrderIOStream& TransByteOrderIOStream::stream_write_to_int(T& int_value)
	{
		const xl::uint32 len = sizeof(int_value);
		if ( _buf_len < len )
		{
			ATLASSERT(FALSE);
			return *this;
		}
		T tmp;
		memcpy( &tmp, _buf_ptr, len );
		int_value = xl::ntoh(tmp);//网络序转为主机序

		_buf_ptr += len;
		_buf_len -= len;
		_bytes_read += len;
		return *this;
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator<<(const backagent_cmd_byte & byte_value)
	{
		return stream_read_int( byte_value );
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator>>(backagent_cmd_byte & byte_value)
	{
		return stream_write_to_int( byte_value );
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator<<(const backagent_cmd_short & short_value)
	{
		return stream_read_int( short_value );
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator>>(backagent_cmd_short & short_value)
	{
		return stream_write_to_int( short_value );
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator<<(const backagent_cmd_int & int_value)
	{
		return stream_read_int( int_value );
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator>>(backagent_cmd_int & int_value)
	{
		return stream_write_to_int( int_value );
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator<<(const backagent_cmd_long & long_value)
	{
		return stream_read_int( long_value );
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator>>(backagent_cmd_long & long_value)
	{
		return stream_write_to_int( long_value );
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator<<(const backagent_cmd_string & string_value)
	{
		xl::uint32 len = static_cast<xl::uint32>(string_value.size());
		*this << len;
		if ( _buf_len < len )
		{
			ATLASSERT(FALSE);
			return *this;
		}
		if ( len != 0 )
		{
			memcpy( _buf_ptr, string_value.c_str(), len );
			_buf_ptr += len;
			_buf_len -= len;
			_bytes_written += len;
		}
		return *this;
	}

	TransByteOrderIOStream & TransByteOrderIOStream::operator>>(backagent_cmd_string & string_value)
	{
		xl::uint32 len = 0;
		*this >> len;
		if ( _buf_len < len )
		{
			ATLASSERT(FALSE);
			return *this;
		}
		if (len != 0)
		{
			string_value.assign( _buf_ptr, len );
			_buf_ptr += len;
			_buf_len -= len;
			_bytes_read += len;
		}
		else
		{
			string_value.erase();
		}
		return *this;
	}

	void TransByteOrderIOStream::copy_byte_to(char *byte_array, xl::uint32 len)
	{
		if (_buf_len < len ) 
		{
			ATLASSERT(FALSE);
			return;
		}

		if (len > 0) 
		{
			memcpy(_buf_ptr,byte_array,len);
			_buf_ptr    += len;
			_buf_len    -= len;
			_bytes_written += len;
		}
	}

	void TransByteOrderIOStream::copy_byte_from(char *byte_array, xl::uint32 len)
	{
		if (_buf_len < len) 
		{
			ATLASSERT(FALSE);
			return;
		}

		if (len > 0) 
		{
			memcpy(byte_array,_buf_ptr,len);
			_buf_ptr      += len;
			_buf_len      -= len;
			_bytes_read   += len;
		}
	}

	void TransByteOrderIOStream::cross_length(xl::uint32 len)
	{
		if ( _buf_len < len )
		{
			ATLASSERT(FALSE);
			return;
		}
		_buf_ptr += len;
		_buf_len -= len;
	}

	void TransByteOrderIOStream::reset_bytes_written()
	{
		_bytes_written = 0;
	}

	void TransByteOrderIOStream::reset_bytes_read()
	{
		_bytes_read = 0;
	}
	void TransByteOrderIOStream::reset()
	{
		reset_bytes_written();
		reset_bytes_read();
		_buf_len = _buf_size;
	}

	xl::uint32 TransByteOrderIOStream::get_bytes_written()
	{
		return _bytes_written;
	}

	xl::uint32 TransByteOrderIOStream::get_bytes_read()
	{
		return _bytes_read;
	}

	char * TransByteOrderIOStream::get_ptr()
	{
		return _buf_ptr;
	}

	xl::uint32 TransByteOrderIOStream::get_len()
	{
		return _buf_len;
	}

//////////////////////////////////////////////////////////////////////////


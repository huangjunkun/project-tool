#ifndef __C_SINGLE_FILE_H
#define __C_SINGLE_FILE_H

#pragma  once

#include <io.h>
#include <exception>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <fstream>
#include <tchar.h>
#include <wchar.h>
#include <cctype>
#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <clocale>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif

#pragma   warning(disable:4127)
/**
** 文件： c_singe_file.h，c_singe_file_test.cpp
**
** 函数： trim_tstring
** 功能： 裁剪字符串，针对字符串首尾裁剪空格与制表符。
** 类型： c_single_file
** 功能： 封装CRT基本的文件读写操作。方便使用。
** 作者：junkun huang  e-mail：huangjunkun@gmail.com
** 日期：2011-07-21 /
*/

namespace junkun
{
/**
** 说明： 一些CRT的宏定义，配置用于多语言_UNICODE程序版本。
*/
	using namespace std;

#ifdef _UNICODE

#define atoi_t		_wtoi
#define itoa_t		_itow
#define strlen_t	wcslen
#define fgets_t		fgetws
#define fputs_t		fputws
#define fscanf_t	fwscanf
#define sscanf_t	swscanf
#define printf_t	wprintf
#define fprintf_t	fwprintf
#define sprintf_t	swprintf
#define tcout		std::wcout
#define tcerr		std::wcerr
#define tcin		std::wcin
#define string_t	std::wstring
#define char_t		wchar_t
#define ofstream_t	std::wofstream
#define ostringstream_t	std::wostringstream
#else

#define atoi_t		atoi
#define itoa_t		_itoa
#define strlen_t	strlen
#define fgets_t		fgets
#define fputs_t		fputs
#define printf_t	printf
#define fscanf_t	fscanf
#define sscanf_t	sscanf
#define fprintf_t	fprintf
#define sprintf_t	sprintf
#define tcout		std::cout
#define tcerr		std::cerr
#define tcin		std::cin
#define string_t	std::string
#define char_t		char
#define ofstream_t	std::ofstream
#define ostringstream_t	std::ostringstream

#endif

#ifndef isdigit_t
#define isdigit_t(ch) (ch >= _T('0') && ch <= _T('9'))
#endif
#ifndef isalpha_t
#define isalpha_t(ch) (((ch) >= _T('a') && (ch) <= _T('z')) || ((ch) >= _T('A') && (ch) <= _T('Z')))
#endif
#ifndef islower_t
#define islower_t(ch) ((ch) >= _T('a') && (ch) <= _T('z'))
#endif
#ifndef isupper_t
#define isupper_t(ch) ((ch) >= _T('A') && (ch) <= _T('Z'))
#endif

const unsigned CHAR_LOWER_UPPER_CHANGE_FACTOR = _T('z') - _T('a');
#ifndef tolower_t
#define tolower_t(ch) (isupper_t(ch) ? ((ch) + CHAR_LOWER_UPPER_CHANGE_FACTOR) : (ch))
//#define tolower_t(ch) isalpha_t(ch) ? (islower_t(ch) ? (ch) : (ch + CHAR_LOWER_UPPER_CHANGE_FACTOR)) : (ch)
#endif
#ifndef toupper_t
#define toupper_t(ch) (islower_t(ch) ? ((ch) - CHAR_LOWER_UPPER_CHANGE_FACTOR) : (ch))
//#define toupper_t(ch) isalpha_t(ch) ? (isupper_t(ch) ? (ch) : (ch + CHAR_LOWER_UPPER_CHANGE_FACTOR)) : (ch)
#endif
	//
	//////////////////////////////////////////////////////////////////////////
static inline string_t localtime2str()
{
	/// #include <windows.h>
// 	SYSTEMTIME st;
// 	::GetLocalTime(&st);
// 	char_t buffer[128];
// 	_stprintf(buffer,_T("%04u-%02u-%02u %02u:%02u:%02u [%u]\r"), st.wYear,st.wMonth,st.wDay,st.wHour ,st.wMinute,st.wSecond);
//
// 	return buffer;
	struct tm *nowtime;
	time_t timer;
	time(&timer);
	nowtime = localtime(&timer);
	char_t buffer[128] = {0};
	if (nowtime)
		sprintf_t(buffer, _T("%04u-%02u-%02u %02u:%02u:%02u"), nowtime->tm_year+1900, nowtime->tm_mon+1, nowtime->tm_mday,
		nowtime->tm_hour, nowtime->tm_min, nowtime->tm_sec);

	return buffer;
}

static inline int cur_thread_id()
{
#if defined(WIN32) || defined(_WIN32)
//#include <windows.h>
    return GetCurrentThreadId();
#elif defined(LINUX) || defined(_LINUX)
    return gettid();
#else
    return 0;
#endif
}
//#define LOG
#ifdef _DEBUG
#ifndef __COUT_FOR_LOG_
#define __COUT_FOR_LOG_

#define cout_trace(msg) do { tcout << _T("[TRACE][") << localtime2str() << _T("][") << cur_thread_id() << _T("]") << __FUNCTION__ << _T("(") << __LINE__ << _T(")") << msg << "\n"; } while(0)
#define cout_debug(msg) do { tcout << _T("[DEBUG][") << localtime2str() << _T("][") << cur_thread_id() << _T("]") << __FUNCTION__ << _T("(") << __LINE__ << _T(")") << msg << "\n"; } while(0)
#define cout_info(msg)  do { tcout << _T("[INFO][") << localtime2str() << _T("][") << cur_thread_id() << _T("]") << __FUNCTION__ << _T("(") << __LINE__ << _T(")" << msg << "\n"; } while(0)

#define cout_trace2(outs, msg) do { outs << _T("[TRACE][") << localtime2str() << _T("][") << cur_thread_id() << _T("]") << __FUNCTION__ << _T("(") << __LINE__ << _T(")") << msg << "\n"; outs.flush(); } while(0)
#define cout_debug2(outs, msg) do { outs << _T("[DEBUG][") << localtime2str() << _T("][") << cur_thread_id() << _T("]") << __FUNCTION__ << _T("(") << __LINE__ << _T(")") << msg << "\n"; outs.flush(); } while(0)
#define cout_info2(outs, msg)  do { outs << _T("[INFO][") << localtime2str() << _T("][") << cur_thread_id() << _T("]") << __FUNCTION__ << _T("(") << __LINE__ << _T(")") << msg << "\n"; outs.flush(); } while(0)

#define assert_false_notify(msg) do { const bool msg(false); assert(msg); } while(0)
#endif // end __COUT_FOR_LOG_

#else

#ifndef __COUT_FOR_LOG_
#define __COUT_FOR_LOG_

#define cout_trace(msg) 0
#define cout_debug(msg) 0
#define cout_info(msg)  0

#define cout_trace2(outs, msg) 0
#define cout_debug2(outs, msg) 0
#define cout_info2(outs, msg)  0

#define assert_false_notify(msg) 0

#endif // end __COUT_FOR_LOG_
#endif // end _DEBUG

}// namespace junkun

namespace junkun
{
/**
** 函数： trim_tstring
** 功能： 裁剪字符串，针对字符串首尾裁剪空格与制表符。
*/

#define SPACE_TCHAR		_T(' ')
#define TAB_TCHAR		_T('\t')
#define RETURN_TCHAR	_T('\n')
	using namespace std;

	template <typename _TChar>
	basic_string<_TChar>  trim_tstring(const basic_string<_TChar>& srcString)
	{//
		if (srcString.empty())
			return basic_string<_TChar>();

		typename basic_string<_TChar>::const_iterator citer = srcString.begin();
		for (; citer != srcString.end(); ++citer)
			if (!(*citer == _TChar(SPACE_TCHAR) || *citer == _TChar(TAB_TCHAR)))
				break;

		typename basic_string<_TChar>::const_reverse_iterator criter = srcString.rbegin();
		for (; criter != srcString.rend(); ++criter)
			if (!(*citer == _TChar(SPACE_TCHAR) || *citer == _TChar(TAB_TCHAR)))
				break;
		typename basic_string<_TChar>::const_iterator end_pos(criter.base());

		return basic_string<_TChar>(citer, end_pos);
	}

/**
** 类型： c_single_file
** 功能： 封装CRT基本的文件读写操作。方便使用。
*/
	class c_single_file
	{
	public:
		//		using namespace std;
	public:
		c_single_file() : _file(0) {}
		c_single_file(const std::string& filename, const std::string& open_mode="wb+")
			: _file(0)
		{
			if ( !open(filename, open_mode))
			{
				assert_false_notify (c_single_file_open_return_false);
				//throw std::logic_error("c_single_file_open_return_false");
			}
		}
		c_single_file(const std::wstring& filename, const std::wstring& open_mode=L"wb+")
			: _file(0)
		{
			if ( !open(filename, open_mode))
			{
				assert_false_notify (c_single_file_open_return_false);
				//throw std::logic_error("c_single_file_open_return_false");
			}
		}

		~c_single_file()
		{
			close();
		}
		// member function
		bool open(const std::string& filename, const std::string& open_type="rb+")
		{
			_file = fopen(filename.c_str(), open_type.c_str());
			return (NULL != _file) ;
		}

		bool open(const std::wstring& filename, const std::wstring& open_type=L"rb+")
		{
			_file = _wfopen(filename.c_str(), open_type.c_str());
			return (NULL != _file) ;
		}

		size_t write(void* pData,size_t len)
		{
			assert(_file);
			return fwrite(pData, 1, len, _file);
		}

		size_t read(void* pData,size_t len)
		{
			assert(_file);
			return fread(pData, 1, len, _file);
		}

		bool close()
		{
			if (_file)
			{
				int res = fclose(_file);
				_file = NULL;
				return (0==res);
			}
			return false;
		}

		int flush()
		{
			assert (_file);
			return fflush(_file);
		}
		char* gets(char* c_str, unsigned len) const
		{
			assert (_file);
			return fgets(c_str, len, _file);
		}
		wchar_t* gets(wchar_t* c_str, unsigned len) const
		{
			assert (_file);
			return fgetws(c_str, len, _file);
		}
		int puts(const char* c_str)
		{
			assert (_file);
			return fputs(c_str, _file);
		}
		int puts(const wchar_t* c_str)
		{
			assert (_file);
			return fputws(c_str, _file);
		}

		int printf(const char * format, ...)
		{
			assert (_file);
			va_list arg_ptr;
			va_start(arg_ptr, format);
			int w_bytes = vfprintf(_file, format, arg_ptr);
			va_end(arg_ptr);
			return w_bytes;
		}

		int printf(const wchar_t * format, ...)
		{
			assert (_file);
			va_list arg_ptr;
			va_start(arg_ptr, format);
			int w_bytes = vfwprintf(_file, format, arg_ptr);
			va_end(arg_ptr);
			return w_bytes;
		}

		int seek(long offset, int origin)
		{
			assert (_file);
			return fseek(_file, offset, origin);
		}

		unsigned tell() const
		{
			assert (_file);
			return ftell(_file);
		}

// 		int seek(__int64 offset, int origin)
// 		{
// 			assert (_file);
// 			return _fseeki64(_file, offset, origin);
// 		}

// 		__int64 tell() const
// 		{
// 			assert (_file);
// 			return _ftelli64(_file);
// 		}
// 		int scanf(const char * format, ...)
// 		{
// 			assert (_file);
// 			va_list arg_ptr;
// 			va_start(arg_ptr, format);
// 			int w_bytes = vfscanf(_file, format, arg_ptr);
// 			va_end(arg_ptr);
// 			return w_bytes;
// 		}
//
// 		int scanf(const wchar_t * format, ...)
// 		{
// 			assert (_file);
// 			va_list arg_ptr;
// 			va_start(arg_ptr, format);
// 			int w_bytes = vfwscanf(_file, format, arg_ptr);
// 			va_end(arg_ptr);
// 			return w_bytes;
// 		}

		int setmode(int mode) { return _setmode(_fileno(_file), mode); }
		//int getmode(int mode) { getmode(_fileno(_file)); }

		int eof() { assert (_file); return feof(_file); }
		int error() { assert (_file); return ferror(_file); }
		bool is_opened() const { return NULL != _file; }
		FILE* file_ptr() { return _file; }
//////////////////////////////////////////////////////////////////////////
		int to_begin()
		{
			return seek(0, SEEK_SET);
			//return rewind(_file);
		}
		int to_end()
		{
			return seek(0, SEEK_END);
		}

		int get_int()
		{
			char_t buf[128];
			char_t* io_ret = gets(buf, sizeof(buf));
			assert (io_ret);
			int ret = 0;
			if (io_ret)
			{
				sscanf_t(buf, _T("%d"), &ret);
			}
//  			int ret = 0;
// 			read(&ret, sizeof(ret));
			return ret;
		}
		// todo: should do some test .
		string_t get_str()
		{
			string_t ret;
			ret.reserve(256);
			char_t* io_ret = NULL;
			const unsigned BUFFER_SIZE = 64;
			string_t tmp;
			tmp.reserve(BUFFER_SIZE);
			do
			{
				tmp.clear();
				char_t buf[BUFFER_SIZE] = {0};
				io_ret = gets(buf, BUFFER_SIZE);
				tmp.assign(buf);
				if (_T('\n') == tmp[tmp.size()-1])
					tmp.resize(tmp.size() - 1);//pass '\n'

				ret += tmp;
			} while (io_ret && tmp.size() == BUFFER_SIZE-1);

			return ret;
		}
	private:
		FILE* _file;
	};//class c_single_file


}// namespace junkun

#endif


// file_exception.cpp: implementation of the file_exception class.
//
//////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include <common/common.h>
#include "file_exception.h"
//#include <boost/lexical_cast.hpp>
#include "utility.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace std;

file_exception::file_exception(const std::string &msg)
				       : io_exception(msg), _error_des(msg),
					   _error_code(0)
{
	_error_des += ", error code: 0";
}

file_exception::file_exception(const std::string &msg, 
	DWORD error_code ) : io_exception(msg), _error_des(msg),
	_error_code(error_code)
{
	_error_des += ", error code: ";
	_error_des += utility::ulong2str(_error_code);	
}

file_exception::~file_exception()
{

}

const char* file_exception::what() const
{
	return _error_des.c_str();
}

bool file_exception::is_disk_full() const
{
	return ( _error_code == ERROR_DISK_FULL );
}
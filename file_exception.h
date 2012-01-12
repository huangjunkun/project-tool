// file_exception.h: interface for the file_exception class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILE_EXCEPTION_H__65E68E82_2817_4503_970D_1601F10F764D__INCLUDED_)
#define AFX_FILE_EXCEPTION_H__65E68E82_2817_4503_970D_1601F10F764D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "io_exception.h"
#include "common.h"

class file_exception : public io_exception  
{
public:
	file_exception(const std::string &msg);
	file_exception(const std::string &msg, DWORD error_code );
	virtual ~file_exception();
	virtual const char* what() const;
	DWORD error_code() const 
	{
		return _error_code;
	}
	bool is_disk_full() const;
	
private:
	DWORD _error_code;
	std::string _error_des;

};

#endif // !defined(AFX_FILE_EXCEPTION_H__65E68E82_2817_4503_970D_1601F10F764D__INCLUDED_)

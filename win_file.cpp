
#include "stdafx.h"
#include "limits.h"
#include "win_file.h"
#include <stdexcept>
//#include <strsafe.h> // for StringCchPrintf


//////////////////////////////////////////////////////////////////////
namespace junkun
{

win_file::win_file(const string_t& file_name, OPENMODE mode )
{
    DWORD access;
    DWORD share_mode;
    if( mode == READ )
    {
        access = GENERIC_READ;
        share_mode = FILE_SHARE_READ|FILE_SHARE_WRITE;
    }
    else
    {
        access = GENERIC_READ|GENERIC_WRITE;
        share_mode = FILE_SHARE_READ;
    }

    DWORD create_flag;
    if( mode == OPEN )
        create_flag = OPEN_EXISTING;
    else if( mode == CREATE )
        create_flag = CREATE_NEW;
    else if( mode == CREATEALWAYS )
        create_flag = CREATE_ALWAYS;
    else
        create_flag = OPEN_EXISTING;

    _file_handle = ::CreateFile(file_name.c_str(), access, share_mode, NULL,
                                create_flag, FILE_ATTRIBUTE_NORMAL, NULL);

    //assert(_file_handle != INVALID_HANDLE_VALUE);
}

win_file::win_file() : _file_handle(INVALID_HANDLE_VALUE)
{
}

win_file::~win_file()
{
    if( _file_handle != INVALID_HANDLE_VALUE )
        ::CloseHandle( _file_handle );
}

void win_file::attach_file_handle( HANDLE file_handle )
{
    if( _file_handle != INVALID_HANDLE_VALUE )
        ::CloseHandle( _file_handle );

    _file_handle = file_handle;
}

HANDLE win_file::detach_file_handle()
{
    HANDLE temp = _file_handle;
    _file_handle = INVALID_HANDLE_VALUE;
    return temp;
}

bool win_file::write( const void *buffer,unsigned int length )
{
    if(!length)
    {
        return false;
    }

    DWORD written;
    if (!::WriteFile(_file_handle, buffer, length, &written, NULL))
    {
        cout_trace(win_file::err_msg());
        return false;
    }

    return (written != length );
}

unsigned int win_file::write_ex( const void* buffer, unsigned int length )
{
    if(!length)
    {
        return 0;
    }

    assert (is_valid());
    DWORD written;
    if (!::WriteFile(_file_handle, buffer, length, &written, NULL))
    {
        // ERROR_INVALID_USER_BUFFER or ERROR_NOT_ENOUGH_MEMORY
        cout_trace(win_file::err_msg());
        return 0;
    }

    return written;
}

bool win_file::read( void *buffer, unsigned int length )
{
    DWORD read_bytes = 0;
    if (!::ReadFile(_file_handle, buffer, length, &read_bytes, NULL))
    {
        cout_trace(win_file::err_msg());
        return false;
    }
    return( read_bytes != length );
}

unsigned int win_file::read_ex( void *buffer, unsigned int length )
{
    DWORD read_bytes;
    if (!::ReadFile(_file_handle, buffer, length, &read_bytes, NULL))
    {
        //assert (false);
        cout_trace(win_file::err_msg());
        return 0;
    }
    return read_bytes;
}

unsigned __int64 win_file::get_pos()
{
    unsigned __int64 pos(0);
    char* pointer = (char*)&pos;
    long high(0);
    long low(0);
    low = ::SetFilePointer(_file_handle, 0, &high, FILE_CURRENT);
    memcpy(pointer, &low,4 );
    memcpy(pointer+4, &high,4 );

    return pos;
}

bool win_file::set_pos( unsigned __int64 pos )
{
    char* pointer=(char*)&pos;
    long high;
    long low;
    memcpy( &low,pointer,4 );
    memcpy( &high,pointer+4,4 );

    int ret = ::SetFilePointer(_file_handle, low, &high, FILE_BEGIN);
    return (INVALID_SET_FILE_POINTER != ret);//-1

// #ifdef _DEBUG
// 	unsigned __int64 new_pos=0;
// 	pointer = (char*)&new_pos;
// 	memcpy( pointer, &ret, 4 );
// 	memcpy( pointer+4, &high, 4 );
// 	if( new_pos != pos )
// 		throw std::runtime_error(" set_positin not correct " );
// #endif
}

bool win_file::set_length(  unsigned __int64 length )
{
    bool ret = set_pos( length );
    return (ret && ::SetEndOfFile( _file_handle ));

}

unsigned __int64 win_file::length()
{
    DWORD low;
    DWORD high;
    low = ::GetFileSize( _file_handle, &high );
    if (low  == (DWORD)-1)
    {
        return static_cast<unsigned __int64>(-1);
    }

    unsigned __int64 length=0;
    char* pointer = (char*)&length;
    memcpy( pointer, &low, 4 );
    memcpy( pointer+4, &high, 4 );

    return length;

}

bool win_file::flush()
{
    return (::FlushFileBuffers(_file_handle) );

}

unsigned __int64 win_file::file_size( const string_t& file_name )
{
// 	WIN32_FILE_ATTRIBUTE_DATA attribute;
// 	if ( GetFileAttributesExA(	file_name.c_str(),
// 								GetFileExInfoStandard,
// 								&attribute ) )
// 	{
// 		return attribute.nFileSizeLow + ( ((unsigned __int64)attribute.nFileSizeHigh) << 32 );
// 	}
// 	else
// 	{
// 		return _UI64_MAX;
// 	}
    // 使用 GetFileAttributesExA 虽然速度快效率高，但是可能因为文件没有打开权限而返回失败
    // 所以为了稳妥起见，使用 FindFirstFile
    WIN32_FIND_DATA data;
    HANDLE handle = ::FindFirstFile( file_name.c_str(), &data );
    if( handle == INVALID_HANDLE_VALUE )
    {
        return _UI64_MAX;
    }
    else
    {
        ::FindClose( handle );
        return data.nFileSizeLow + ( ((unsigned __int64)data.nFileSizeHigh) << 32 );
    }
}

bool win_file::is_exist( const string_t& file_name )
{
// 	WIN32_FILE_ATTRIBUTE_DATA attribute;
// 	BOOL ret = GetFileAttributesExA(	file_name.c_str(),
// 		GetFileExInfoStandard,
// 		&attribute );
// 	if( ret )
// 		return true;
// 	else
// 	{
// 		DWORD error_code = ::GetLastError();
// 		if( error_code == ERROR_FILE_NOT_FOUND )
// 			return false;
// 		else
// 			return true;
// 	}

    WIN32_FIND_DATA data;
    HANDLE handle = ::FindFirstFile( file_name.c_str(), &data );
    if( handle == INVALID_HANDLE_VALUE )
    {
        return false;
    }
    else
    {
        ::FindClose( handle );
        return true;
    }
}

bool win_file::rename( const string_t& old_name, const string_t& new_name )
{
    return (::MoveFile(old_name.c_str(), new_name.c_str()) );

}

bool win_file::remove( const string_t& file_name )
{
    if (is_dir(file_name))
    {
        return rm_dir(file_name);
    }
    else
        return (::DeleteFile( file_name.c_str() ) );
}

bool win_file::rm_dir( const string_t& full_path )
{
    assert (is_dir(full_path));
    std::vector<string_t> file_list;
    path_included_file_list(full_path, file_list);

    for (unsigned i = 0; i < file_list.size(); ++i)
    {
        string_t delete_file = full_path + file_list[i];
        if (is_dir(delete_file))
        {
            if (_T('\\') != delete_file[delete_file.size()-1])
                delete_file.push_back(_T('\\'));

            rm_dir(delete_file);
        }
        else
        {
            ::DeleteFile(delete_file.c_str());
            cout_trace( "delete file: " << delete_file);
        }
    }
    cout_trace( "delete dir: " << full_path);
    return ::RemoveDirectory(full_path.c_str());
}

bool win_file::is_dir( const string_t& full_path )
{
    DWORD attr = GetFileAttributes(full_path.c_str());
    if( attr == 0xFFFFFFFF )
    {
        return false;
    }
    return ((FILE_ATTRIBUTE_DIRECTORY&attr)!=0);
}

bool win_file::create_dir( const string_t& full_path )
{
    return ::CreateDirectory(full_path.c_str(), NULL);
}

bool win_file::remove_dir(const string_t& full_path )
{
    return ::RemoveDirectory(full_path.c_str());
}

bool win_file::copy( const string_t& source_name, const string_t& dest_name, bool overwrite_if_exists )
{
    if (!overwrite_if_exists && is_exist(dest_name))
        return false;

    return (::CopyFile(source_name.c_str(), dest_name.c_str(), FALSE) );
}

string_t win_file::err_msg()
{
    LPVOID lpMsgBuf;
    DWORD dw = error();

    ::FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

    // Display the error message and exit the process

// 	LPVOID lpDisplayBuf;
    /// 111
//     lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
//                                       (lstrlen((LPCTSTR)lpMsgBuf)+40)*sizeof(TCHAR));
//
//     StringCchPrintf((LPTSTR)lpDisplayBuf,
//                     LocalSize(lpDisplayBuf),
//                     _T("error %d: .err_msg: %s."), dw, lpMsgBuf);
    /// 222
    const unsigned iBuffLen = (lstrlen((LPCTSTR)lpMsgBuf)+64);
    char_t* lpDisplayBuf = new char_t[iBuffLen];
    //memset(lpDisplayBuf, 0, iBuffLen);
    sprintf_t(lpDisplayBuf, _T("error:%d err_msg:%ls"), dw, lpMsgBuf);
    string_t ret(lpDisplayBuf);
    //LocalFree(lpDisplayBuf);
    delete[] lpDisplayBuf;
    LocalFree(lpMsgBuf);

    return ret;

}

}///namespace junkun

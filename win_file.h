


#ifndef WIN_FILE_H_INCLUDED
#define WIN_FILE_H_INCLUDED


#include<string>
#include <Windows.h>
#include "c_single_file.h"

/**
** 源文件： win_file.h, win_file.cpp, win_file_test.cpp
** 功能说明：
** 类型win_file封装Win32-API文件操作，完成一组基本的文件操作功能包含：
** #1 win_file对象自动创建或打开，关闭文件。文件读写，设置位置。
** #2 静态方法有创建目录，删除目录，判断目录，删除文件，复制文件，文件重命名。
** #3 注意，在文件操作返回失败时，建议调用error()或err_msg()获取具体的错误信息.
** #4：操作系统：windows 7 专业版
** 开发+测试环境：VS2003 + CodeBlocks10.02
** 作者：junkun huang  e-mail：huangjunkun@gmail.com
** 日期：2011-09-02 /
*/

namespace junkun
{
using namespace std;

class   win_file
{
public:
    enum OPENMODE { OPEN = 0, CREATE = 1, CREATEALWAYS = 2, READ = 3 };

    win_file(const string_t& file_name, OPENMODE mode );
    win_file();
    virtual ~win_file();

    bool write( const void* buffer, unsigned int length );
    unsigned int write_ex( const void* buffer, unsigned int length );
    bool read( void* buffer, unsigned int length );
    unsigned int read_ex( void *buffer, unsigned int length );
    unsigned __int64 get_pos();
    bool set_pos( unsigned __int64 pos );
    bool set_length( unsigned __int64 length );
    unsigned __int64 length();
    bool flush();
    void attach_file_handle( HANDLE file_handle );
    HANDLE detach_file_handle();
    bool is_valid()
    {
        return INVALID_HANDLE_VALUE != _file_handle;
    }

    static unsigned __int64 file_size( const string_t& file_name ); // 返回文件的长度,如果文件不存在或者打开失败,则返回 _UI64_MAX

    static bool is_exist( const string_t& file_name );
    static bool rename( const string_t& old_name, const string_t& new_name );
    static bool remove( const string_t& file_name );
    static bool rm_dir( const string_t& full_path );
    static bool is_dir( const string_t& full_path );
    static bool create_dir( const string_t& full_path );
    static bool remove_dir(const string_t& full_path );
    static bool copy( const string_t& source_name, const string_t& dest_name, bool overwrite_if_exists=false);

    static int error()
    {
        return ::GetLastError();
    }
    static string_t err_msg();
private:
    HANDLE _file_handle;
};


template <typename _TChar>
bool path_included_file_list(const basic_string<_TChar>& dir, vector<basic_string<_TChar> >& result)
{
    typedef basic_string<_TChar> _TString;

    assert (!dir.empty() && _T('\\') == dir[dir.size() - 1]);

    WIN32_FIND_DATA	win32_find_data;
    const static basic_string<_TChar> ALL_FILE_INCLUDED(_T("*"));
    HANDLE handle =	::FindFirstFile(_TString(dir+ALL_FILE_INCLUDED).c_str(), &win32_find_data);
    if (INVALID_HANDLE_VALUE == handle)
    {
        tcerr << _T("INVALID_HANDLE_VALUE == handle.\n");
        return false;
    }

    while (0 != ::FindNextFile(handle, &win32_find_data))
    {
        const _TString file_name(win32_find_data.cFileName);
        if (_T(".." != file_name))
            result.push_back(file_name);//wsDirectory+
    }
    FindClose(handle);
    return ERROR_NO_MORE_FILES == GetLastError();
}

} /// namespace junkun


#endif // WIN_FILE_H_INCLUDED

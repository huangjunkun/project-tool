


#ifndef WIN_FILE_H_INCLUDED
#define WIN_FILE_H_INCLUDED


#include<string>
#include <Windows.h>
#include "c_single_file.h"

/**
** Դ�ļ��� win_file.h, win_file.cpp, win_file_test.cpp
** ����˵����
** ����win_file��װWin32-API�ļ����������һ��������ļ��������ܰ�����
** #1 win_file�����Զ�������򿪣��ر��ļ����ļ���д������λ�á�
** #2 ��̬�����д���Ŀ¼��ɾ��Ŀ¼���ж�Ŀ¼��ɾ���ļ��������ļ����ļ���������
** #3 ע�⣬���ļ���������ʧ��ʱ���������error()��err_msg()��ȡ����Ĵ�����Ϣ.
** #4������ϵͳ��windows 7 רҵ��
** ����+���Ի�����VS2003 + CodeBlocks10.02
** ���ߣ�junkun huang  e-mail��huangjunkun@gmail.com
** ���ڣ�2011-09-02 /
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

    static unsigned __int64 file_size( const string_t& file_name ); // �����ļ��ĳ���,����ļ������ڻ��ߴ�ʧ��,�򷵻� _UI64_MAX

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

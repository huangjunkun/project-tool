// vc2003_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "../project-tool/win_file.h"
//#include "
using namespace std;
using namespace junkun;
int _tmain(int argc, _TCHAR* argv[])
{
	{
	//cout_trace(win_file::err_msg());
	const string_t TEST_DIR = _T("c:\\temp\\win_file_test\\");
	const string_t TEST_FILE_NAME = _T("c:\\temp\\win_file_test\\test_file");
	const string_t TEST_FILE_RENAME = _T("c:\\temp\\win_file_test\\re_test_file");
	const string_t TEST_FILE_COPY = _T("c:\\temp\\win_file_test\\cp_test_file");
	const string_t TEST_DIR_NAME = _T("c:\\temp\\win_file_test\\test_dir\\");
	
	cout_trace("test_dir remove:" << win_file::remove(TEST_DIR));
	cout_trace("test_dir make_dir:" << win_file::create_dir(TEST_DIR));

	{
		char_t buff[1024];
		const unsigned buff_len = sizeof(buff);//
		//memset_t(buff, _T('A'), buff_len);
		//strset()
		win_file test_file(TEST_FILE_NAME, win_file::CREATE);
		cout_trace("test_file is_valid:" << test_file.is_valid());
		//cout_trace(win_file::err_msg());
		cout_trace("test_file write_ex:" << test_file.write_ex(buff, buff_len*sizeof(char_t)));
		cout_trace("test_file flush:" << test_file.flush());
		cout_trace("test_file length:" << test_file.length());
		cout_trace("test_file get_position:" << test_file.get_pos());//
		cout_trace("test_file set_position(100):" << test_file.set_pos(100));
		cout_trace("test_file get_position:" << test_file.get_pos());
	}

	cout_trace("test_file file_size:" << win_file::file_size(TEST_FILE_NAME));
	cout_trace("test_file is_exist:" << win_file::is_exist(TEST_FILE_NAME));
	cout_trace("test_file copy:" << win_file::copy(TEST_FILE_NAME, TEST_FILE_COPY));
	cout_trace("test_file rename:" << win_file::rename(TEST_FILE_NAME, TEST_FILE_RENAME));
	//cout_trace(win_file::err_msg());
	cout_trace("test_dir is_dir:" << win_file::is_dir(TEST_DIR_NAME));
	cout_trace("test_dir make_dir:" << win_file::create_dir(TEST_DIR_NAME));
	cout_trace("test_dir is_dir:" << win_file::is_dir(TEST_DIR_NAME));
	}

	/**     */
	cout << "\n Hello, World !" << endl;
	//system("pause");
	return 0;
}


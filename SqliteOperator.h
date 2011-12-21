#pragma once
#ifndef _SQLITE_QUERY_
#define _SQLITE_QUERY_

#include <string>
#include <cassert>

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#endif

#include "sqlite/sqlite3.h"

/**
** 源文件： SqliteOperator.h, SqliteOperator.cpp, SqliteOperatorTest.cpp
** 功能说明：
** 封装sqlite原生的数据库操作API。可以简便完成执行SQL语句，操作包括创建，插入，更新，删除，查询数据等。
** 详细见下面类型CSqliteOperator的定义。还定义其他的sqlite辅助类型，包括数据库类型，事务类型，锁类型。

** 作者：junkun huang  e-mail：huangjunkun@gmail.com
** 日期：2011-12-02 /
*/

/// sqlite 原生类型声明。数据操作状态与数据对象。
struct sqlite3_stmt;
struct sqlite3;

/// 辅助类型CSqliteDb的实现。
class SqliteDbImpl
{
public:
	friend class CSqliteDb;
	SqliteDbImpl( const wchar_t* dbPath);
	~SqliteDbImpl();
	bool SqliteOK() const { return SQLITE_OK == _error_code; }
private:
	void add_ref() { ++_ref_count; }
	void sub_ref();
	SqliteDbImpl(const SqliteDbImpl&);
	SqliteDbImpl& operator=(const SqliteDbImpl&);
private:
	sqlite3* _sqlite_db;  //
	int _ref_count;
	int _error_code;
};

/// 辅助类型，抽象原生sqlite对象。
/// TODO：实现临时数据库，即内存数据库！
class CSqliteDb
{
private:
	SqliteDbImpl* _db_impl;
public:
	CSqliteDb(const wchar_t* dbPath );
	CSqliteDb();
	void Open(const wchar_t* dbPath);
	void Close();
	~CSqliteDb();
	CSqliteDb(const CSqliteDb& sqliteDb);
	CSqliteDb& operator=(const CSqliteDb& sqliteDb);
	operator const sqlite3*() const throw()
	{
		return _db_impl->_sqlite_db;
	}
	operator sqlite3*() throw()
	{
		return _db_impl->_sqlite_db;
	}
	sqlite3* GetDBPtr() const
	{
		return _db_impl->_sqlite_db;
	}
	bool IsTableExist(const std::wstring& tb_name);
	int _error_code;
};

/// sqlite操作类型，包含基本的sqlite数据操作。主要功能包含如下：
/// 1. 绑定sql语句包含的可变参数；
/// 2. 编译sql语句，准备执行；
/// 3. 执行sql语句，区分查询与其他操作。区分点为是否改变数据库。
/// 4. 还有，一些数据常用属性的访问。
class CSqliteOperator
{
public:
	typedef struct tagText
	{
		wchar_t* str;
		int      length;
		tagText()
			:str(NULL), length(0)
		{}
		tagText(wchar_t* p,int l)
			:str(p), length(l)
		{}
	} Text;

	typedef struct tagBlob
	{
		void*    ptr;
		int      length;
		tagBlob()
			:ptr(NULL), length(0)
		{}
		tagBlob(void* p,int l)
			:ptr(p), length(l)
		{}
	} Blob;

	CSqliteOperator();
	CSqliteOperator(CSqliteDb& sqlite_db);
	~CSqliteOperator(void);

	sqlite3_stmt* PrepareSQL(const std::wstring& sql);

	CSqliteOperator& operator<<(int i);
	CSqliteOperator& operator<<(float f);
	CSqliteOperator& operator<<(double d);
	CSqliteOperator& operator<<(__int64 i64);
	CSqliteOperator& operator<<(const Text& text);
	CSqliteOperator& operator<<(const wchar_t* text);
	CSqliteOperator& operator<<(const Blob& blog);
	CSqliteOperator& operator<<(const std::wstring& text);
	CSqliteOperator& operator>>(int& i);
	CSqliteOperator& operator>>(float& f);
	CSqliteOperator& operator>>(double& d);
	CSqliteOperator& operator>>(__int64& i64);
	CSqliteOperator& operator>>(Text& text);
	CSqliteOperator& operator>>(std::wstring& text);
	CSqliteOperator& operator>>(Blob& blob);
	void Reset();
	bool Next();
	bool Execute();
	CSqliteDb* SqliteDbPtr() { return _sqlite_db_ptr; }

	std::string LibVersion() { return sqlite3_libversion(); }
	std::string DatabaseName(int n) { return sqlite3_column_database_name(_stmt_ptr,n); }
	std::string TableName(int n) { return sqlite3_column_table_name(_stmt_ptr,n); }
	std::string ColumnName(int n) { return sqlite3_column_origin_name(_stmt_ptr,n); }
	std::string ColumnName2(int n) { return sqlite3_column_name(_stmt_ptr, n); }
	int ClearBindings() { return sqlite3_clear_bindings(_stmt_ptr); }

	int ColumnCount() { return sqlite3_column_count(_stmt_ptr); }
	int DataCount() { return sqlite3_data_count(_stmt_ptr); }
	int Errcode()  { return sqlite3_errcode(*_sqlite_db_ptr); }
	const char* Errmsg()  { return sqlite3_errmsg(*_sqlite_db_ptr); }

	static void FilterSingleQuote(std::wstring& text);
protected:
	void _Finalize();
	int  _Step();
private:
	int				_cur_bind;
	sqlite3_stmt*	_stmt_ptr;
	CSqliteDb*		_sqlite_db_ptr;
	int				_error_code;
protected:
	//LOG_CLS_DEC();
};

/// 封装sqlite事务对象类型，作用于简便提交事务操作！
class AutoTransaction
{
public:
	AutoTransaction(sqlite3* db_ptr,bool exclusive = false);
	~AutoTransaction();
	void Commit();
private:
	bool m_commited;
	sqlite3* m_db_ptr;
};

/// 封装sqlite互斥对象类型，作用于数据库操作时加锁解锁！
class CSqliteMutex
{
	/**
	SQLITE_MUTEX_FAST
	SQLITE_MUTEX_RECURSIVE
	SQLITE_MUTEX_STATIC_MASTER
	SQLITE_MUTEX_STATIC_MEM
	SQLITE_MUTEX_STATIC_MEM2
	SQLITE_MUTEX_STATIC_PRNG
	SQLITE_MUTEX_STATIC_LRU
	SQLITE_MUTEX_STATIC_LRU2
	**/
public:
	CSqliteMutex(int mutex_type = SQLITE_MUTEX_FAST)
		: _mutex(0)
	{
		_mutex = sqlite3_mutex_alloc(mutex_type);
		assert(NULL != _mutex);
	}
	virtual ~CSqliteMutex()
	{
		if (_mutex)
			sqlite3_mutex_free(_mutex);
	}
	CSqliteMutex( const CSqliteMutex& mx );
	CSqliteMutex& operator=( const CSqliteMutex& mx );

	void lock()
	{
		if (_mutex)
			sqlite3_mutex_enter(_mutex);
	}
	void unlock()
	{
		if (_mutex)
			sqlite3_mutex_leave(_mutex);
	}

	bool try_lock( unsigned wait_millisec = 0 )
	{
#if defined(_WIN32) || defined(WIN32)
		unsigned start_time = ::GetTickCount();
		if (_mutex)
		{
			do
			{
				int try_res = sqlite3_mutex_try(_mutex);
				if (SQLITE_OK == try_res)
					return true;
				assert(SQLITE_BUSY == try_res);

				if (::GetTickCount() - start_time < wait_millisec)
				{
					::Sleep(1);
				}
				else
					break;
			} while (true);
		}
#else
	#error "not implement"

#endif
		return false;
// 		if (_mutex)
// 		{
// 			do
// 			{
// 				int try_res = sqlite3_mutex_try(_mutex);
// 				if (SQLITE_OK == try_res)
// 					return true;
// 				ATLASSERT(SQLITE_BUSY == try_res);
//
// 				if ((wait_millisec--) > 0)
// 					::Sleep(1);
// 				else
// 					break;
// 			} while (true);
// 		}
// 		return false;
	}

	operator sqlite3_mutex*() const
	{
		return _mutex;
	}
private:
	sqlite3_mutex*	_mutex;
};
#endif





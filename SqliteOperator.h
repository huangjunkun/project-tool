#pragma once
#ifndef _SQLITE_QUERY_
#define _SQLITE_QUERY_

#include <string>
#include <cassert>

#if defined(_WIN32) || defined(WIN32)
#include <windows.h>
#endif

#include "sqlite/sqlite3.h"

struct sqlite3_stmt;
struct sqlite3;
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





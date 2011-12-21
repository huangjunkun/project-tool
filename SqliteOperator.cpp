//#include "stdafx.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include "./SqliteOperator.h"


//////////////////////////////////////////////////////////////////////////
CSqliteOperator::CSqliteOperator(CSqliteDb& sqlite_db )
	: _cur_bind(0),_stmt_ptr(0),_sqlite_db_ptr(0)
{
	_sqlite_db_ptr = &sqlite_db;
}

CSqliteOperator::CSqliteOperator()
	: _cur_bind(0),_stmt_ptr(0),_sqlite_db_ptr(0)
{
}

CSqliteOperator::~CSqliteOperator(void)
{
	_Finalize();
}

void CSqliteOperator::Reset()
{
	_cur_bind = 0;
	if(_stmt_ptr)
		sqlite3_reset(_stmt_ptr);
}

bool CSqliteOperator::Next()
{
	return SQLITE_ROW == _Step();
}

void CSqliteOperator::_Finalize()
{
	_cur_bind  = 0;
	if(_stmt_ptr)
	{
		int res = sqlite3_finalize(_stmt_ptr);
		assert(res == SQLITE_OK);
		_stmt_ptr = NULL;
	}
}

CSqliteOperator& CSqliteOperator::operator<<( int i )
{
	if(_stmt_ptr)
	{
		sqlite3_bind_int(_stmt_ptr,++_cur_bind,i);
	}
	return *this;
}



CSqliteOperator& CSqliteOperator::operator<<( float f )
{
	double d = f;
	*this<<d;
	return *this;
}

CSqliteOperator& CSqliteOperator::operator<<( double d )
{
	if(_stmt_ptr)
	{
		sqlite3_bind_double(_stmt_ptr,++_cur_bind,d);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator<<( __int64 i64 )
{
	if(_stmt_ptr)
	{
		sqlite3_bind_int64(_stmt_ptr,++_cur_bind,i64);
	}
	return *this;
}


CSqliteOperator& CSqliteOperator::operator<<(const Text& text)
{
	if(_stmt_ptr)
	{
		//std::wstring t = text.str;
		//sqlite3_bind_text16(_stmt_ptr,++_cur_bind,t.c_str(),(int)t.length() * sizeof(wchar_t),SQLITE_TRANSIENT);
		sqlite3_bind_text16(_stmt_ptr, ++_cur_bind, text.str, text.length * sizeof(wchar_t),SQLITE_TRANSIENT);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator<<(const wchar_t* text)
{
	if(_stmt_ptr)
	{
		//std::wstring t = text;
		//sqlite3_bind_text16(_stmt_ptr,++_cur_bind,t.c_str(),(int)t.length() * sizeof(wchar_t),SQLITE_TRANSIENT);
		size_t len = wcslen(text);
		sqlite3_bind_text16(_stmt_ptr,++_cur_bind, text, len * sizeof(wchar_t) ,SQLITE_TRANSIENT);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator<<(const std::wstring& text)
{
	if(_stmt_ptr)
	{
		const std::wstring& t = text;
		sqlite3_bind_text16(_stmt_ptr,++_cur_bind,t.c_str(),(int)t.length() * sizeof(wchar_t),SQLITE_TRANSIENT);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator<<(const Blob& blob)
{
	if(_stmt_ptr)
	{
		sqlite3_bind_blob(_stmt_ptr,++_cur_bind,blob.ptr,blob.length,SQLITE_TRANSIENT);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(int& i)
{
	if(_stmt_ptr)
	{
		i = sqlite3_column_int(_stmt_ptr,_cur_bind++);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(float& f)
{
	if(_stmt_ptr)
	{
		double d;
		d = sqlite3_column_double(_stmt_ptr,_cur_bind++);
		f = (float)d;
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(double& d)
{
	if(_stmt_ptr)
	{
		d = sqlite3_column_double(_stmt_ptr,_cur_bind++);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(__int64& i64)
{
	if(_stmt_ptr)
	{
		i64 = sqlite3_column_int64(_stmt_ptr,_cur_bind++);
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(Text& text)
{
	if(_stmt_ptr)
	{
		wchar_t* t = (wchar_t*)sqlite3_column_text16(_stmt_ptr,_cur_bind++);
		if(t)
		{
// 			wcsncpy(text.str, t, text.length-1);
// 			text.str[text.length-1] = 0;
			std::wstring str(t);
			assert(text.length > str.length());
			wcsncpy(text.str, str.c_str(), str.length());
			text.str[str.length()] = 0;
		}
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(std::wstring& text)
{
	if(_stmt_ptr)
	{
		wchar_t* t = (wchar_t*)sqlite3_column_text16(_stmt_ptr,_cur_bind++);
		if( t )
		{
			text.assign(t);
		}
	}
	return *this;
}

CSqliteOperator& CSqliteOperator::operator>>(Blob& blob)
{
	if(_stmt_ptr)
	{
		int data_length = sqlite3_column_bytes(_stmt_ptr,_cur_bind);
		data_length = data_length < blob.length ? data_length : blob.length;
		memcpy(blob.ptr,sqlite3_column_blob(_stmt_ptr,_cur_bind++),data_length);
	}
	return *this;
}


sqlite3_stmt* CSqliteOperator::PrepareSQL( const std::wstring& sql)
{
	_Finalize();//Reset();
	sqlite3_stmt* statement = NULL;
	if(_sqlite_db_ptr)
	{
		if( 0 != sqlite3_prepare16_v2(static_cast<sqlite3*>(*_sqlite_db_ptr),sql.c_str(),(int)sql.length() * 2, &statement,NULL) )
		{
			statement = NULL;
		}
	}
	_stmt_ptr = statement;
	return statement;
}

void CSqliteOperator::FilterSingleQuote( std::wstring& text )
{
	size_t lookHere = 0;
	size_t findHere = 0;
	static const  std::wstring doubleSignleQuote = L"''";
	while( (findHere = text.find('\'',lookHere)) != std::wstring::npos)
	{
		text.replace(findHere,1,doubleSignleQuote);
		lookHere = findHere + 2;
	}
}

bool CSqliteOperator::Execute()
{
	int r = _Step();
	if (r == SQLITE_BUSY)
	{
		r = _Step();//考虑再给一次执行机会
	}
	//ATLASSERT(r == SQLITE_DONE);
	Reset();
	return r == SQLITE_DONE;
}

int CSqliteOperator::_Step()
{
	if( !_stmt_ptr )
		return SQLITE_ERROR;
	_cur_bind = 0;
	return  sqlite3_step(_stmt_ptr);
}
//////////////////////////////////////////////////////////////////////////
SqliteDbImpl::SqliteDbImpl( const wchar_t* dbPath )
{
	_error_code = sqlite3_open16(dbPath,&_sqlite_db);
	_ref_count = 1;
}

SqliteDbImpl::~SqliteDbImpl()
{
	sub_ref();
}

void SqliteDbImpl::sub_ref()
{
	--_ref_count;
	if(_ref_count == 0)
	{
		_error_code = sqlite3_close(_sqlite_db);
		_sqlite_db = NULL;
	}
}
//////////////////////////////////////////////////////////////////////////
CSqliteDb::CSqliteDb()
{
	_db_impl = NULL;
}

void CSqliteDb::Open(const wchar_t* dbPath)
{
	if(_db_impl)
		_db_impl->sub_ref();

	if (_db_impl)   delete _db_impl;
	_db_impl = new SqliteDbImpl(dbPath);
}

void CSqliteDb::Close()
{
	if(_db_impl)
		_db_impl->sub_ref();
	_db_impl = NULL;
}

CSqliteDb::CSqliteDb( const wchar_t* dbPath )
{
	_db_impl = new SqliteDbImpl(dbPath);
}

CSqliteDb::CSqliteDb( const CSqliteDb& sqliteDb )
{
	_db_impl = sqliteDb._db_impl;
	_db_impl->add_ref();
}
CSqliteDb::~CSqliteDb()
{
	if ( _db_impl != NULL )
	{
		_db_impl->sub_ref();
		if (_db_impl)   delete _db_impl;
	}
}

CSqliteDb& CSqliteDb::operator=( const CSqliteDb& sqliteDb )
{
	if(&sqliteDb != this)
	{
		sqliteDb._db_impl->add_ref();
		_db_impl->sub_ref();
		_db_impl = sqliteDb._db_impl;
	}
	return *this;
}

bool CSqliteDb::IsTableExist(const std::wstring& tb_name)
{
	CSqliteOperator query(*this);
	query.PrepareSQL(L"select tbl_name from sqlite_master where tbl_name = ? ");
	query<<tb_name;
	return query.Next();
}
//////////////////////////////////////////////////////////////////////////

AutoTransaction::AutoTransaction(sqlite3* db_ptr,bool exclusive)
	: m_commited(false), m_db_ptr(db_ptr)
{
	std::string  exclusive_term = exclusive ? "EXCLUSIVE " : "";
	std::string  query = "BEGIN " + exclusive_term + " TRANSACTION;";
	int exe_res;
    do
	{
		exe_res = sqlite3_exec((m_db_ptr), query.c_str(), 0, 0, 0);
	}while (exe_res == SQLITE_BUSY);

    if (SQLITE_OK != exe_res)
    {
        m_commited = true;
    }

}

AutoTransaction::~AutoTransaction()
{
	if (!m_commited)
        sqlite3_exec((m_db_ptr), "ROLLBACK TRANSACTION;", 0, 0, 0);

}

void AutoTransaction::Commit()
{
	if (!m_commited)
    {
        for (int exe_res;;)
        {
            exe_res = sqlite3_exec((m_db_ptr), "END TRANSACTION;", 0, 0, 0);//or "COMMIT TRANSACTION"
            if (exe_res != SQLITE_BUSY)
                break;
		}
		m_commited = true;
	}
}

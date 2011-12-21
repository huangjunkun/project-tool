
#include <cstdio>
#include <iostream>

using namespace std;
#include "./main.h"

#include "sqlite/sqlite3.h"
#include "./SqliteOperator.h"

static int _sql_callback(void* notused, int argc, char ** argv, char** szColName)
{
    int i;
    for ( i=0;  i < argc;  i++ )
    {
        printf( "%s = %s\n", szColName[i], argv[i] == 0 ? "NUL" : argv[i]);

    }
    return 0;
}

int main()
{
    {
    CSqliteDb sqlite_db;
    sqlite_db.Open(L"F:\\SQLite\\TestSQLite\\test-old.db");
    bool is_table_exist = sqlite_db.IsTableExist(L"users") ;
    std::cout << " " << is_table_exist << "\n";
    assert(is_table_exist);

    CSqliteOperator sqlite_operator(sqlite_db);
    #define SELECT(field, less_value) do { \
        sqlite_operator.PrepareSQL(L"select * from users where age>=?2;");  \
        sqlite_operator << field << less_value; \
        while (sqlite_operator.Next()) \
        { \
            std::wstring pName; \
            int		age; \
            std::wstring pBirthday; \
            sqlite_operator >> pName >> age >> pBirthday; \
            std::wcout << pName << ", " << age << ", " << pBirthday << "\n"; \
            std::wcout << " --  --  --  --  --  --  --  --  --  --  --\n";\
        } \
        }while(0)

    SELECT(L"age", 90);
    cout << " ##########################\n";
    sqlite_operator.PrepareSQL(L"insert into users values(?1, ?2, ?3)");
    sqlite_operator << L"chen" << 99 << L"1900-01-01";
    cout << " sqlite_operator.Execute(): " << sqlite_operator.Execute() << "\n";

    sqlite_operator.PrepareSQL(L"update users set age=?1 where userid=?2;");
    sqlite_operator << 88 << L"cheng";
    cout << " sqlite_operator.Execute(): " << sqlite_operator.Execute() << "\n";


    //SELECT(L"age", 20);

    }
/**
    {
	//sqlite3* pDb;
    using namespace XlCommon;
    using XlCommon::CSqliteDb;

	CSqliteDb sqlitedb;
	//sqlitedb.Attach(pDb);

	sqlitedb.Open(L"F:\\SQLite\\TestSQLite\\test.db");
	assert(sqlitedb.GetLastError()  == SQLITE_OK);

	std::cout << " " << sqlitedb.IsTableExist(L"students") << "\n";
	std::cout << " " << sqlitedb.IsTableExist(L"users") << "\n";

	CSqliteBindingManipulation sqlManipulation; // 非查询SQL
	sqlitedb.PrepareManipulation(L"insert into users values(\"cheng\", 21, \"1987-05-08\");", sqlManipulation);
	if (sqlManipulation.Execute())
		std::cout << "insert ok !\n";
	else
	{
		std::wcout << sqlManipulation.GetLastErrorMessage() << "\n";
		std::cout << "insert false !\n";
	}

	sqlitedb.PrepareManipulation(L"update users set age=?1 where userid=?2;", sqlManipulation);
	sqlManipulation.Bind(1, 22);
	std::wstring userid = L"li";
	sqlManipulation.Bind(2, userid.c_str(), userid.length());
	if (sqlManipulation.Execute())
		std::cout << "update ok !\n";
	else
	{
		std::wcout << sqlManipulation.GetLastErrorMessage() << "\n";
		std::cout << "update false !\n";
	}

	CSqliteBindingQuery sqlquery; //查询SQL
	sqlitedb.PrepareQuery(L"select * from users where ?2 >= ?1;", sqlquery);
	sqlquery.Bind(1, 20);
	sqlquery.Bind(2, L"age", wcslen(L"age"));

	if (sqlquery.Execute())
	{
		//printf("%d \n",sqlquery.GetColumnCount());

		while(!sqlquery.IsEof())
		{
			LPCWSTR pName;
			int		age;
			LPCWSTR pBirthday;

			pName = sqlquery.GetColumnString(0);
			age = sqlquery.GetColumnInt(1);
			pBirthday = sqlquery.GetColumnText(2);
			//std::wcout << sqlquery.GetColumnType(2) << "\n";

// 			if (SQLITE_BLOB == sqlquery.GetColumnType(3))
// 				std::cout << "SQLITE_BLOB == sqlquery.GetColumnType(3) \n";
			std::wcout << pName << ", " << age << ", " << pBirthday << "\n";
			assert(TRUE == sqlquery.MoveToNextRow());
			sqlquery.MoveToNextRow();
		}
	}


	sqlManipulation.Finalize();
	sqlquery.Finalize();

	sqlitedb.Close();
    }
/**
    {
    const wchar_t* pWStr = L"Hello\n";
    std::cout << pWStr;
    std::wcout << pWStr;
    }
/**
    const char* sSQL1 = "create table users(userid varchar(20) PRIMARY KEY, age int, birthday datetime);";
    const char* sSQL2 = "insert into users values('wang',20,'1989-5-4'), values('huang',20,'1988-6-7');";
    const char* sSQL3 = "select * from users;";

    sqlite3* db = 0;
    char* pErrMsg = 0;
    int ret = 0;

    // 连接数据库
    ret = sqlite3_open("./test.db", &db);//sqlite3_open16, sqlite3_open_v2


    if ( ret != SQLITE_OK )
    {
    fprintf(stderr, "无法打开数据库: %s\n", sqlite3_errmsg(db));
    return(1);
    }
    printf("数据库连接成功!\n");
    // 执行建表SQL
    sqlite3_exec( db, sSQL1, 0, 0, &pErrMsg );
    if ( ret != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %sn", pErrMsg);
        sqlite3_free(pErrMsg);
    }

    // 执行插入记录SQL
    sqlite3_exec( db, sSQL2, 0, 0, &pErrMsg);

    if ( ret != SQLITE_OK )
    {
        fprintf(stderr, "SQL error: %sn", pErrMsg);
        sqlite3_free(pErrMsg);
    }
    // 查询数据表
    sqlite3_exec( db, sSQL3, _sql_callback, 0, &pErrMsg);

    // 关闭数据库
    sqlite3_close(db);
    db = 0;

    /**         */
    cout << "Hello world!" << endl;
    return 0;
}

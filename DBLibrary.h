#pragma once
#include "stdafx.h"
#include "CLog.h"

#define ODBC_DRIVER L"{MySQL ODBC 8.0 Unicode Driver}"
#define BUFFER_LEN_SHORT 32
#define BUFFER_LEN_MIDDLE 512
#define BUFFER_LEN_BIG 1024

class DBLibaray
{
private:
	MYSQL* m_Con;
	WCHAR m_szServerIP[BUFFER_LEN_SHORT];
	WCHAR m_szUserID[BUFFER_LEN_SHORT];
	WCHAR m_szPassword[BUFFER_LEN_SHORT];

private:
	BOOL Initial();

	void GetColumnName(const char* tableName, std::list<WCHAR*>* list, std::stack<int>* stack);

	void PrintColumnName(const char* tableName);

	void PrintSelectAll(std::list<WCHAR*>* list, int row, int column);

public:
	DBLibaray()
	{
		m_Con = mysql_init(NULL);
		m_szServerIP[0] = '\0';
		m_szUserID[0] = '\0';
		m_szPassword[0] = '\0';
		Initial();
	}

	DBLibaray(const WCHAR* ip, const WCHAR* id, const WCHAR* pass)
	{
		m_Con = mysql_init(NULL);
		StringCchCopy(m_szServerIP, BUFFER_LEN_SHORT, ip);
		StringCchCopy(m_szUserID, BUFFER_LEN_SHORT, id);
		StringCchCopy(m_szPassword, BUFFER_LEN_SHORT, pass);
		Initial();
	}

	~DBLibaray()
	{
		mysql_close(m_Con);
	}

	BOOL MySQLSendQuery(char* query);



	BOOL SelectAllQuery(const WCHAR* tableName, int* row, int* column, std::list<WCHAR*>* list);

	// Insert, delete용
	BOOL InsertQuery(const WCHAR* tableName, const WCHAR* szStringFormat,...);

	// 테이블과 조건
	BOOL DeleteQuery(const WCHAR* tableName, const WCHAR* szStringFormat, ...);
};








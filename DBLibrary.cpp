#include "stdafx.h"
#include "DBLibrary.h"

BOOL DBLibaray::Initial()
{
	setlocale(LC_ALL, "");

	char charIp[64];
	char charUser[64];
	char charPass[64];
	WideCharToMultiByte(CP_ACP, 0, m_szServerIP, -1, charIp, 64, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, m_szUserID, -1, charUser, 64, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, m_szPassword, -1, charPass, 64, NULL, NULL);

	if (m_Con == nullptr)
	{
		fprintf(stderr, "%s\n", mysql_error(m_Con));		//에러 프린트, printf("%s")와 같음
		LOG(L"MYSQL", LOG_ERROR, L"mysql_init() error. Error no : %d", mysql_errno(m_Con));
		Sleep(1000);
		exit(1);
	}

	if (!(mysql_real_connect(m_Con, charIp, charUser, charPass, NULL, 0, NULL, 0) == NULL))
	{
		wprintf(L"\n접속 성공");
		mysql_set_character_set(m_Con, "euckr");
	}
	else
	{
		fprintf(stderr, "연결 오류 : %s\n", mysql_error(m_Con));
		getchar();
	}

	return TRUE;
}

BOOL DBLibaray::MySQLSendQuery(char* query)
{
	MYSQL_RES* sql_res;
	mysql_query(m_Con, query);
	sql_res = mysql_store_result(m_Con);
	if (sql_res != 0)
	{
		return FALSE;
	}
	return TRUE;
}
BOOL DBLibaray::SelectAllQuery(const WCHAR* tableName, int* row, int* column, std::list<WCHAR*>* list)
{
	MYSQL_RES* sql_res;
	char queryCh[BUFFER_LEN_BIG] = "SELECT * FROM ";
	char tableCh[BUFFER_LEN_MIDDLE] = { '\0' };
	int result = 0;
	
	WideCharToMultiByte(CP_ACP, 0, tableName, -1, tableCh, BUFFER_LEN_BIG, NULL, NULL);
	StringCchCatA(queryCh, BUFFER_LEN_BIG, tableCh);
	int i = 0;
	int k = 0;
	result = mysql_query(m_Con, queryCh);
	if (result != 0)
	{
		LOG(L"MYSQL", LOG_CONSOLE, L"mysql_query() fail, error : %s", mysql_error(m_Con));
		return false;
	}
	sql_res = mysql_store_result(m_Con);
	*column = mysql_field_count(m_Con);
	char*** rowArray = (char***)malloc(sizeof(char**) * mysql_field_count(m_Con));
	while (rowArray[i++] = mysql_fetch_row(sql_res));
	*row = i - 1;
	for (int i = 0; i < *row; i++)
	{
		for (int j = 0; j < *column; j++)
		{
			WCHAR* temparray = (WCHAR*)malloc(sizeof(WCHAR)*64);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, rowArray[i][j], strlen(rowArray[i][j]), temparray, 64);
			temparray[strlen(rowArray[i][j])] = '\0';
			(list[i]).push_back(temparray);
		}
	}
	PrintColumnName(tableCh);
	PrintSelectAll(list, *row, *column);
	mysql_free_result(sql_res);


	return true;
}


BOOL DBLibaray::InsertQuery(const WCHAR* tableName, const WCHAR* szStringFormat, ...)
{
	char queryCh[BUFFER_LEN_BIG] = "INSERT INTO ";
	char tableNameCh[BUFFER_LEN_SHORT];
	char columnNameCh[BUFFER_LEN_MIDDLE] = "(";
	std::list<WCHAR*> columnName;	
	std::stack<int> columnSkip;
	WideCharToMultiByte(CP_ACP, 0, tableName, -1, tableNameCh, BUFFER_LEN_BIG, NULL, NULL);
	StringCchCatA(queryCh, BUFFER_LEN_BIG, tableNameCh);

	GetColumnName(tableNameCh, &columnName, &columnSkip);

	int i = 0;
	for (std::list<WCHAR*>::iterator itor = columnName.begin(); itor != columnName.end(); itor++)
	{
		if (!columnSkip.empty())
		{
			if (i == columnSkip.top())
			{
				columnSkip.pop();
				continue;
			}
		}
		char columnTemp[BUFFER_LEN_SHORT];
		WideCharToMultiByte(CP_ACP, 0, *itor, -1, columnTemp, BUFFER_LEN_SHORT, NULL, NULL);
		StringCchCatA(columnNameCh, BUFFER_LEN_MIDDLE, "`");
		StringCchCatA(columnNameCh, BUFFER_LEN_MIDDLE, columnTemp);
		StringCchCatA(columnNameCh, BUFFER_LEN_MIDDLE, "`, ");
	}
	columnNameCh[strlen(columnNameCh) - 2] = ')';
	StringCchCatA(queryCh, BUFFER_LEN_BIG, columnNameCh);
	StringCchCatA(queryCh, BUFFER_LEN_BIG, " VALUES (");

	HRESULT hResult;
	va_list va;
	WCHAR szInMessage[1024]{ '\0' };
	char szInMessageCh[1024]{ '\0' };

	va_start(va, szStringFormat);
	hResult = StringCchVPrintf(szInMessage, 256, szStringFormat, va);
	va_end(va);
	WideCharToMultiByte(CP_ACP, 0, szInMessage, -1, szInMessageCh, BUFFER_LEN_BIG, NULL, NULL);

	StringCchCatA(queryCh, BUFFER_LEN_BIG, szInMessageCh);
	StringCchCatA(queryCh, BUFFER_LEN_BIG, " )");


	//WideCharToMultiByte(CP_ACP, 0, query, -1, queryCh, BUFFER_LEN_BIG, NULL, NULL);
	int result = 0;
	result = mysql_query(m_Con, queryCh);
	if (result != 0)
	{
		LOG(L"MYSQL", LOG_CONSOLE, L"mysql_query() fail, error : %s", mysql_error(m_Con));
		return false;
	}
	return true;
}

BOOL DBLibaray::DeleteQuery(const WCHAR* tableName, const WCHAR* szStringFormat, ...)
{
	int rows = 0;
	MYSQL_RES* sql_res;
	char queryCh[BUFFER_LEN_BIG] = "DELETE FROM ";
	char tableNameCh[BUFFER_LEN_SHORT];

	WideCharToMultiByte(CP_ACP, 0, tableName, -1, tableNameCh, BUFFER_LEN_BIG, NULL, NULL);
	StringCchCatA(queryCh, BUFFER_LEN_BIG, tableNameCh);
	StringCchCatA(queryCh, BUFFER_LEN_BIG, " WHERE ");

	HRESULT hResult;
	va_list va;
	WCHAR szInMessage[1024]{ '\0' };
	char szInMessageCh[1024]{ '\0' };

	va_start(va, szStringFormat);
	hResult = StringCchVPrintf(szInMessage, 256, szStringFormat, va);
	va_end(va);
	WideCharToMultiByte(CP_ACP, 0, szInMessage, -1, szInMessageCh, BUFFER_LEN_BIG, NULL, NULL);
	StringCchCatA(queryCh, BUFFER_LEN_BIG, szInMessageCh);

	int result = 0;
	result = mysql_query(m_Con, queryCh);
	if (result != 0)
	{
		LOG(L"MYSQL", LOG_CONSOLE, L"mysql_query() fail, error : %s", mysql_error(m_Con));
		return false;
	}
	return true;
}

void DBLibaray::GetColumnName(const char* tableName, std::list<WCHAR*>* list, std::stack<int>* stack)
{
	int rows = 0;
	MYSQL_RES* sql_res;
	char queryCh[BUFFER_LEN_BIG] = "SHOW COLUMNS FROM ";
	StringCchCatA(queryCh, BUFFER_LEN_BIG, tableName);
	mysql_query(m_Con, queryCh);
	sql_res = mysql_store_result(m_Con);
	char*** rowArray = (char***)malloc(sizeof(char**) * mysql_field_count(m_Con));
	while (rowArray[rows++] = mysql_fetch_row(sql_res));
	for (int i = 0; i < rows - 1; i++)
	{
		WCHAR* temparray = (WCHAR*)malloc(sizeof(WCHAR) * 64);
		if (strcmp(rowArray[i][5], "auto_increment") == 0)
		{
			stack->push(i);
		}
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, rowArray[i][0], strlen(rowArray[i][0]), temparray, 64);
		temparray[strlen(rowArray[i][0])] = '\0';
		(*list).push_back(temparray);
	}
}

void DBLibaray::PrintColumnName(const char* tableName)
{
	int rows = 0;
	MYSQL_RES* sql_res;
	char queryCh[BUFFER_LEN_BIG] = "SHOW COLUMNS FROM ";
	StringCchCatA(queryCh, BUFFER_LEN_BIG, tableName);
	mysql_query(m_Con, queryCh);
	sql_res = mysql_store_result(m_Con);
	char*** rowArray = (char***)malloc(sizeof(char**) * mysql_field_count(m_Con));
	while (rowArray[rows++] = mysql_fetch_row(sql_res));
	printf("%s\n", queryCh);
	wprintf(L"\n");
	for (int i = 0; i < rows - 1; i++)
	{
		wprintf(L"|------------|");
	}
	wprintf(L"\n");
	for (int i = 0; i < rows - 1; i++)
	{
		WCHAR* temparray = (WCHAR*)malloc(sizeof(WCHAR) * 64);
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, rowArray[i][0], strlen(rowArray[i][0]), temparray, 64);
		temparray[strlen(rowArray[i][0])] = '\0';
		wprintf(L"| %10s |", temparray);
	}
	wprintf(L"\n");
	for (int i = 0; i < rows - 1; i++)
	{
		wprintf(L"|------------|");
	}
	mysql_free_result(sql_res);

}

void DBLibaray::PrintSelectAll(std::list<WCHAR*>* list, int row, int column)
{
	wprintf(L"\n");
	for (int i = 0; i < row; i++)
	{
		for (std::list<WCHAR*>::iterator itor = list[i].begin(); itor != list[i].end(); itor++)
		{
			wprintf(L"| %10s |", *itor);
		}
		wprintf(L"\n");
	}
	for (int i = 0; i < column; i++)
	{
		wprintf(L"|------------|");
	}
}

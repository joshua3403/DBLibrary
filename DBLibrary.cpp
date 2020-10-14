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
		fprintf(stderr, "%s\n", mysql_error(m_Con));		//���� ����Ʈ, printf("%s")�� ����
		LOG(L"MYSQL", LOG_ERROR, L"mysql_init() error. Error no : %d", mysql_errno(m_Con));
		Sleep(1000);
		exit(1);
	}

	if (!(mysql_real_connect(m_Con, charIp, charUser, charPass, NULL, 0, NULL, 0) == NULL))
	{
		wprintf(L"\n���� ����");
		mysql_set_character_set(m_Con, "euckr");
	}
	else
	{
		fprintf(stderr, "���� ���� : %d\n", mysql_error(m_Con));
		getchar();
	}

	return TRUE;
}

void DBLibaray::MySQLSelectAll(const WCHAR* tableName, int* row, int* column, std::list<WCHAR*>* list)
{
	MYSQL_RES* sql_res;
	char queryCh[BUFFER_LEN_BIG] = "SELECT * FROM ";
	char tableCh[BUFFER_LEN_MIDDLE] = { '\0' };
	
	WideCharToMultiByte(CP_ACP, 0, tableName, -1, tableCh, BUFFER_LEN_BIG, NULL, NULL);
	StringCchCatA(queryCh, BUFFER_LEN_BIG, tableCh);
	int i = 0;
	int k = 0;
	mysql_query(m_Con, queryCh);
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


	return;
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
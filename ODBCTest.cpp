#include "stdafx.h"


// SQLȯ�� �ڵ�
SQLHENV hEnv;
// SQL���� �ڵ�
SQLHDBC hDbc;
// SQL��� �ڵ�
SQLHSTMT hStmt;
HRESULT hResult;

MYSQL *con;



int main()
{
	setlocale(LC_ALL, "");

	WCHAR State[1024];
	WCHAR Message[1024];
	SQLSMALLINT MsgLen;
	int NativeError;

	TCHAR BuffID[40];
	TCHAR BuffPass[40];
	TCHAR BuffNickName[40];
	TCHAR BuffScore[40];

	SQLLEN lBuffID, lBuffPass, lBuffNickname, lBuffScore;

	// ���̳ʸ�
	SQLLEN Size = SQL_LEN_DATA_AT_EXEC(512);
	SQLPOINTER Token;

	// ȯ�漳���� �Ҵ��ϰ� ���� �Ӽ��� �����Ѵ�.
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS)
		return 0;
	if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)
		return 0;

	// �����ڵ��� �Ҵ��ϰ� �����Ѵ�.
	if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS)
		return 0;

	// ���� �ڵ鿡 ���� ���� �޽��� ó��
	memset(Message, 0, 1024);
	MsgLen = 0;
	SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);
	wprintf(L"%s\n", Message);

	// �����ϱ�
	WCHAR szConnect[1024] = L"Driver={MySQL ODBC 8.0 Unicode Driver};Server=127.0.0.1;Database=project_a;User=root;Password=Tjsdyd12!@;";
	SQLTCHAR OutCon[255];
	SQLSMALLINT cbOutCon;

	SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)szConnect, wcslen(szConnect), OutCon, sizeof(OutCon) / 2, &cbOutCon, SQL_DRIVER_NOPROMPT);

	// ���ӿ� ���� ���� �޽��� ó��
	SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);
	wprintf(L"%s\n", Message);

	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS)
		return false;

	SQLSMALLINT colcount = 0;


	// �����ϱ�(Select�� ���ε� ���)
	SQLExecDirect(hStmt, (SQLTCHAR*)L"SELECT * FROM player", SQL_NTS);

	SQLNumResultCols(hStmt, &colcount);
	SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);

	// ���ε� �ϱ�
	SQLBindCol(hStmt, 1, SQL_C_TCHAR, BuffID, 40, (SQLLEN*)&lBuffID);
	SQLBindCol(hStmt, 2, SQL_C_TCHAR, BuffPass, 40, (SQLLEN*)&lBuffPass);
	SQLBindCol(hStmt, 3, SQL_C_TCHAR, BuffNickName, 40, (SQLLEN*)&lBuffNickname);
	SQLBindCol(hStmt, 4, SQL_C_TCHAR, BuffScore, 40, (SQLLEN*)&lBuffScore);
	SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);

	// ��� ���
	while (SQLFetch(hStmt) != SQL_NO_DATA)
	{
		wprintf(L"ID:%s, Password:%s, Nickname:%s, Score:%s\n", BuffID, BuffPass, BuffNickName, BuffScore);

		SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);
	}

	// ������
	SQLCloseCursor(hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}
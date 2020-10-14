#include "stdafx.h"


// SQL환경 핸들
SQLHENV hEnv;
// SQL연결 핸들
SQLHDBC hDbc;
// SQL명령 핸들
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

	// 바이너리
	SQLLEN Size = SQL_LEN_DATA_AT_EXEC(512);
	SQLPOINTER Token;

	// 환경설정을 할당하고 버전 속성을 설정한다.
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS)
		return 0;
	if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER) != SQL_SUCCESS)
		return 0;

	// 접속핸들을 할당하고 연결한다.
	if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS)
		return 0;

	// 접속 핸들에 대한 에러 메시지 처리
	memset(Message, 0, 1024);
	MsgLen = 0;
	SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);
	wprintf(L"%s\n", Message);

	// 접속하기
	WCHAR szConnect[1024] = L"Driver={MySQL ODBC 8.0 Unicode Driver};Server=127.0.0.1;Database=project_a;User=root;Password=Tjsdyd12!@;";
	SQLTCHAR OutCon[255];
	SQLSMALLINT cbOutCon;

	SQLDriverConnect(hDbc, NULL, (SQLTCHAR*)szConnect, wcslen(szConnect), OutCon, sizeof(OutCon) / 2, &cbOutCon, SQL_DRIVER_NOPROMPT);

	// 접속에 대한 에러 메시지 처리
	SQLGetDiagRec(SQL_HANDLE_DBC, hDbc, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);
	wprintf(L"%s\n", Message);

	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS)
		return false;

	SQLSMALLINT colcount = 0;


	// 실행하기(Select문 바인딩 출력)
	SQLExecDirect(hStmt, (SQLTCHAR*)L"SELECT * FROM player", SQL_NTS);

	SQLNumResultCols(hStmt, &colcount);
	SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);

	// 바인딩 하기
	SQLBindCol(hStmt, 1, SQL_C_TCHAR, BuffID, 40, (SQLLEN*)&lBuffID);
	SQLBindCol(hStmt, 2, SQL_C_TCHAR, BuffPass, 40, (SQLLEN*)&lBuffPass);
	SQLBindCol(hStmt, 3, SQL_C_TCHAR, BuffNickName, 40, (SQLLEN*)&lBuffNickname);
	SQLBindCol(hStmt, 4, SQL_C_TCHAR, BuffScore, 40, (SQLLEN*)&lBuffScore);
	SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);

	// 결과 출력
	while (SQLFetch(hStmt) != SQL_NO_DATA)
	{
		wprintf(L"ID:%s, Password:%s, Nickname:%s, Score:%s\n", BuffID, BuffPass, BuffNickName, BuffScore);

		SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, 1, (SQLTCHAR*)State, (SQLINTEGER*)&NativeError, (SQLTCHAR*)Message, 1024, &MsgLen);
	}

	// 마무리
	SQLCloseCursor(hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	SQLDisconnect(hDbc);
	SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
	SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
}
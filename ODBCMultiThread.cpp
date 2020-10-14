#include "stdafx.h"
#include "DBLibrary.h"

#define SERVERIP L"127.0.0.1"
#define USERID L"root"
#define PASSWORD L"Tjsdyd12!@"

int main()
{
	DBLibaray* pDB = new DBLibaray(SERVERIP, USERID, PASSWORD);
	std::list<WCHAR*> test[1000];
	int row = 0, col = 0;

	pDB->MySQLSelectAll(L"project_a.player", &row, &col, test);


	
	return 0;
}
#include "stdafx.h"
#include "DBLibrary.h"
#include "Queue(LockFree).h"
#include "MemoryPool(LockFree).h"
#include "Protocol.h"

#define SERVERIP L"127.0.0.1"
#define USERID L"root"
#define PASSWORD L"Tjsdyd12!@"

DBLibaray* pDB = new DBLibaray(SERVERIP, USERID, PASSWORD);
CLFFreeList<st_DBQUERY_MSG_PLAYER> QueryPool;
CQueue<st_DBQUERY_MSG_PLAYER*> DBQuery;
HANDLE hThread[3];
HANDLE hEvent;

char g_IdArray[][10] = { {"joshua"},{"Fernandes"}, {"Erika"}, {"Derik"} };

__int64 g_iMessageCount;
__int64 g_iQueryCount;

unsigned int WINAPI MakeMessage(LPVOID lpParam);
unsigned int WINAPI WorkerThread(LPVOID lpParam);
unsigned int WINAPI PritThread(LPVOID lpParam);

BOOL Flag = FALSE;

int main()
{
	hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	hThread[0] = (HANDLE)_beginthreadex(NULL, 0, MakeMessage, NULL, 0, NULL);
	hThread[1] = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, NULL, 0, NULL);
	hThread[2] = (HANDLE)_beginthreadex(NULL, 0, PritThread, NULL, 0, NULL);

	while (true)
	{
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			Flag = TRUE;
			break;
		}
	}

	WaitForMultipleObjects(3, hThread, TRUE, INFINITE);

	return 0;
}

unsigned int __stdcall MakeMessage(LPVOID lpParam)
{
	srand((unsigned int)GetCurrentThreadId());
	int i = 0;
	while (!Flag)
	{
		st_DBQUERY_MSG_PLAYER* msg = QueryPool.Alloc();
		WORD type = rand() % 2;
		if (type == 0)
		{
			msg->Type = df_DBQUERY_PLAYER;
			msg->iAccountNo = 1;
			int nameCase = rand() % 4;
			switch (nameCase)
			{
			case 0:
				StringCchCopyA(msg->szID, 20, g_IdArray[0]);
				break;
			case 1:
				StringCchCopyA(msg->szID, 20, g_IdArray[1]);
				break;
			case 2:
				StringCchCopyA(msg->szID, 20, g_IdArray[2]);
				break;
			case 3:
				StringCchCopyA(msg->szID, 20, g_IdArray[3]);
				break;
			}
		}
		else if (type == 1)
		{
			((st_DBQUERY_MSG_ITEM*)msg)->Type = df_DBQUERY_ITEM;
			((st_DBQUERY_MSG_ITEM*)msg)->iItemNo = 1;
			((st_DBQUERY_MSG_ITEM*)msg)->szAttack = rand() % 300;
		}
		g_iMessageCount++;
		DBQuery.Enqueue(msg);
		SetEvent(hEvent);
	}
	return 0;
}

unsigned int __stdcall WorkerThread(LPVOID lpParam)
{
	DWORD result = 0;

	while (!Flag)
	{

		result = WaitForSingleObject(hEvent, INFINITE);
		if (result == WAIT_OBJECT_0)
		{
			st_DBQUERY_MSG_PLAYER* Msg;
			DBQuery.Dequeue(&Msg);
			char Query[128];
			switch (Msg->Type)
			{
			case df_DBQUERY_PLAYER:
				StringCchPrintfA(Query, 128, "UPDATE project_a.player SET `id` = '%s' WHERE `accountno` = '%d'", Msg->szID, Msg->iAccountNo);
				pDB->MySQLSendQuery(Query);
				break;
			case df_DBQUERY_ITEM:
				StringCchPrintfA(Query, 128, "UPDATE project_a.item SET `attack` = '%d' WHERE `itemno` = '%d'", ((st_DBQUERY_MSG_ITEM*)Msg)->szAttack, ((st_DBQUERY_MSG_ITEM*)Msg)->iItemNo);
				pDB->MySQLSendQuery(Query);
				break;
			}
			QueryPool.Free(Msg);
			g_iQueryCount++;

			ResetEvent(hEvent);
		}
		Sleep(10);
	}

	while (DBQuery.GetUsingCount() != 0) 
	{
		st_DBQUERY_MSG_PLAYER* Msg;
		DBQuery.Dequeue(&Msg);
		char Query[128];
		switch (Msg->Type)
		{
		case df_DBQUERY_PLAYER:
			StringCchPrintfA(Query, 128, "UPDATE project_a.player SET `id` = '%s' WHERE `accountno` = '%d'", Msg->szID, Msg->iAccountNo);
			pDB->MySQLSendQuery(Query);
			g_iQueryCount++;
			break;
		case df_DBQUERY_ITEM:
			StringCchPrintfA(Query, 128, "UPDATE project_a.item SET `attack` = '%d' WHERE `itemno` = '%d'", ((st_DBQUERY_MSG_ITEM*)Msg)->szAttack, ((st_DBQUERY_MSG_ITEM*)Msg)->iItemNo);
			pDB->MySQLSendQuery(Query);
			g_iQueryCount++;
			break;
		}
	}
	wprintf(L"MSG QueueSize : %lld", DBQuery.GetUsingCount());
	return 0;
}

unsigned int __stdcall PritThread(LPVOID lpParam)
{
	wprintf(L"\n");
	while (!Flag)
	{
		wprintf(L"g_iMessageCount : %lld\n", g_iMessageCount);
		wprintf(L"QueryTPS : %lld\n", g_iMessageCount);
		wprintf(L"MSG QueueSize : %lld\n", DBQuery.GetUsingCount());

		g_iMessageCount = 0;
		g_iMessageCount = 0;
		Sleep(1000);
	}
 	return 0;
}

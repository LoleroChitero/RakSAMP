/*
	Updated to 0.3.7 by P3ti
*/

#include "main.h"
#include <fcntl.h>
#include <io.h>
#include <iostream>
#include <fstream>

char szInputBuffer[512];

DWORD WINAPI inputThread(PVOID p)
{
	HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD dwRead;

	while(1)
	{
		memset(szInputBuffer, 0, 512);

		SetConsoleMode(hStdInput, ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
		FlushConsoleInputBuffer(hStdInput);
		ReadConsole(hStdInput, szInputBuffer, 512, &dwRead, NULL);

		RunCommand(szInputBuffer, 0);

		Sleep(5);
	}

	return 0;
}

BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
	switch(fdwCtrlType) 
	{
		case CTRL_C_EVENT: 
		case CTRL_CLOSE_EVENT: 
		case CTRL_BREAK_EVENT: 
		case CTRL_LOGOFF_EVENT: 
		case CTRL_SHUTDOWN_EVENT: 
			sampDisconnect(0);
			ExitProcess(0);
			return TRUE;

		default: 
			return FALSE; 
	} 
} 

void SetUpConsole()
{
	int hConHandle;
	long lStdHandle;
	FILE *fp;

	AllocConsole();

	// output
	lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stdout = *fp;
	setvbuf( stdout, NULL, _IONBF, 0 );

	// input
	lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "r" );
	*stdin = *fp;
	setvbuf( stdin, NULL, _IONBF, 0 );

	// error
	lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
	hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
	fp = _fdopen( hConHandle, "w" );
	*stderr = *fp;
	setvbuf( stderr, NULL, _IONBF, 0 );

	std::ios::sync_with_stdio();

	SetConsoleTitle("RakSAMP " RAKSAMP_VERSION);
	CreateThread(NULL, 0, inputThread, NULL, 0, NULL);

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
}

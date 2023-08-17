#include "process.h"

#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>

bool IsProcessRunning(const wchar_t* processName)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	PROCESSENTRY32W processEntry;
	processEntry.dwSize = sizeof(PROCESSENTRY32W);

	if (Process32FirstW(hSnapshot, &processEntry))
	{
		do
		{
			if (wcscmp(processEntry.szExeFile, processName) == 0)
			{
				CloseHandle(hSnapshot);
				return true;
			}
		} while (Process32NextW(hSnapshot, &processEntry));
	}

	CloseHandle(hSnapshot);
	return false;
}
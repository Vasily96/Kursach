
#include <windows.h>
#include <stdio.h>
#include "..\fmifs.h"
#define _UNICODE 1
#include "tchar.h"


VOID Usage(PWCHAR ProgramName)
{
	_tprintf(L"�������������: %s [drive:] [-F] [-V] [-R] [-C]\n\n");
	_tprintf(L"  [drive:]    ���������� ����, ����� ���������.\n");
	_tprintf(L"  -F         ����������� ������ �� �����.\n");
	_tprintf(L"  -V         ����������� ���� ������� ����� �� �����.\n");
	_tprintf(L"  -R          ���������� ������ �������� � �������������� ��� ����� ������.\n");
	_tprintf(L"  -C          ��������� ����, ������ ���� �� ���������.\n");
	_tprintf(L"\n");
}
void PrintWin32Error(PWCHAR Message, DWORD ErrorCode)
{
	LPVOID lpMsgBuf;

	FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, ErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(PWCHAR)&lpMsgBuf, 0, NULL);
	_tprintf(L"%s: %s\n", Message, lpMsgBuf);
	LocalFree(lpMsgBuf);
}

BOOLEAN __stdcall ChkdskCallback(CALLBACKCOMMAND Command, DWORD Modifier, PVOID Argument)
{
	PDWORD percent;
	PBOOLEAN status;
	PTEXTOUTPUT output;

	
	
	switch (Command) {

	case PROGRESS:
		percent = (PDWORD)Argument;
		_tprintf(L"%d percent completed.\r", *percent);
		break;

	case OUTPUT:
		output = (PTEXTOUTPUT)Argument;
		fprintf(stdout, "%s", output->Output);
		break;

	case DONE:
		status = (PBOOLEAN)Argument;
		if (*status == TRUE) {

			_tprintf(L"Chkdsk was unable to complete successfully.\n\n");
			Error = TRUE;
		}
		break;
	}
	return TRUE;
}



BOOLEAN LoadFMIFSEntryPoints()
{
	LoadLibrary("fmifs.dll");

	if (!(Chkdsk = (void *)GetProcAddress(GetModuleHandle("fmifs.dll"),
		"Chkdsk"))) {

		return FALSE;
	}
	return TRUE;
}
int wmain(int argc, WCHAR *argv[])
{
	int badArg;
	HANDLE volumeHandle;
	WCHAR fileSystem[1024];
	WCHAR volumeName[1024];
	DWORD serialNumber;
	DWORD flags, maxComponent;

	

	
	if (!LoadFMIFSEntryPoints()) {

		_tprintf(L"�� ������� ���������� ����� ����� FMIFS.\n\n");
		return -1;
	}

	
	if ((badArg = ParseCommandLine(argc, argv))) {

		_tprintf(L"����������� �������� : %s\n", argv[badArg]);

		Usage(argv[0]);
		return -1;
	}
}
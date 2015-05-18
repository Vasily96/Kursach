
#include <windows.h>
#include <stdio.h>
#include "..\fmifs.h"
#define _UNICODE 1
#include "tchar.h"


VOID Usage(PWCHAR ProgramName)
{
	_tprintf(L"»спользование: %s [drive:]  [-R] [-C]\n\n");
	_tprintf(L"  [drive:]    ќпредел€ет диск, чтобы проверить.\n");
	;
	_tprintf(L"  -R          Ќахождение плохих секторов и восстановление той части данных.\n");
	_tprintf(L"  -C          ѕровер€ет диск, только если он загр€знен.\n");
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

int ParseCommandLine(int argc, WCHAR *argv[])
{
	int i;
	BOOLEAN gotFix = FALSE;

	BOOLEAN gotClean = FALSE;



	for (i = 1; i < argc; i++) {

		switch (argv[i][0]) {

		case '-':
		case '/':

			switch (argv[i][1]) {


			case L'R':
			case L'r':

				if (gotFix) return i;
				ScanSectors = TRUE;
				gotFix = TRUE;
				break;

			case L'C':
			case L'c':

				if (gotClean) return i;
				SkipClean = TRUE;
				gotClean = TRUE;
				break;

			default:
				return i;
			}
			break;

		default:

			if (Drive) return i;
			if (argv[i][1] != L':') return i;

			Drive = argv[i];
			break;
		}
	}
	return 0;
}


//------------------------------------------------------------
BOOLEAN __stdcall ChkdskCallback(CALLBACKCOMMAND Command, PVOID Argument)
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


//---------------------------------------------------------------------
int wmain(int argc, WCHAR *argv[])
{
	int badArg;
	HANDLE volumeHandle;
	WCHAR fileSystem[1024];
	WCHAR volumeName[1024];
	DWORD serialNumber;
	DWORD flags, maxComponent;



	
	if (!LoadFMIFSEntryPoints()) {

		_tprintf(L"Could not located FMIFS entry points.\n\n");
		return -1;
	}

	
	if ((badArg = ParseCommandLine(argc, argv))) {

		_tprintf(L"Unknown argument: %s\n", argv[badArg]);


		return -1;
	}

	
	if (!Drive) {

		if (!GetCurrentDirectoryW(sizeof(CurrentDirectory), CurrentDirectory)) {//Retrieves the current directory for the current process

			PrintWin32Error(L"Could not get current directory", GetLastError());
			return -1;
		}

	}
	else {

		wcscpy(CurrentDirectory, Drive);
	}
	CurrentDirectory[2] = L'\\';
	CurrentDirectory[3] = (WCHAR)0;
	Drive = CurrentDirectory;

	
	if (!GetVolumeInformationW(Drive,
		volumeName, sizeof(volumeName),
		&serialNumber, &maxComponent, &flags,
		fileSystem, sizeof(fileSystem))) {

		PrintWin32Error(L"Could not query volume", GetLastError());
		return -1;
	}

	
	if (FixErrors) {

		swprintf(volumeName, L"\\\\.\\%C:", Drive[0]);
		volumeHandle = CreateFileW(volumeName, GENERIC_WRITE,
			0, NULL, OPEN_EXISTING,
			0, 0);
		if (volumeHandle == INVALID_HANDLE_VALUE) {

			_tprintf(L"Chdskx cannot run because the volume is in use by another process.\n\n");
			return -1;
		}
		CloseHandle(volumeHandle);

		
		SetConsoleCtrlHandler(CtrlCIntercept, TRUE);
	}

	
	_tprintf(L"The type of file system is %s.\n", fileSystem);
	Chkdsk(Drive, fileSystem, FixErrors, Verbose, SkipClean, ScanSectors,
		NULL, NULL, ChkdskCallback);

	if (Error) return -1;
	return 0;
}
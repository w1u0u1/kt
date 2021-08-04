#include "sysinfo.h"


WIN_VERSION_INFO sysinfo_os_version()
{
	RTL_OSVERSIONINFOW ver;
	
	RtlGetVersion(&ver);

	switch (ver.dwMajorVersion)
	{
	case 5:
	{
		switch (ver.dwMinorVersion)
		{
		case 0:
			return WIN_VERSION_2K;
			break;
		case 1:
			return WIN_VERSION_XP;
			break;
		case 2:
			return WIN_VERSION_2K3;
			break;
		}
	}
	break;
	case 6:
	{
		switch (ver.dwMinorVersion)
		{
		case 0:
			return WIN_VERSION_VISTA_2008;
			break;
		case 1:
			return WIN_VERSION_WIN7_2008R2;
			break;
		case 2:
			return WIN_VERSION_WIN8_2012;
			break;
		case 3:
			return WIN_VERSION_WIN81_2012R2;
			break;
		}
	}
	break;
	case 10:
		return WIN_VERSION_WIN10;
		break;
	}

	return WIN_VERSION_NONE;
}

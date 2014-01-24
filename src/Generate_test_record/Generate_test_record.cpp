#include <ObjBase.h>
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include "md5_engine.h"
using namespace std;

std::string white_list = "white_list.log";
std::string gray_list = "gray_list.log";
std::string black_list = "black_list.log";

const char* newGUID()
{
	static char buf[64] = {0};
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(buf, sizeof(buf)
			, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
		, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
		, guid.Data4[6], guid.Data4[7]
		);
	}
	return (const char*)buf;
}

void write(HANDLE handle, const string &md5)
{
	string strPrint ;
	strPrint = md5 + "\r\n";
	DWORD writed;
	BOOL b = ::WriteFile(handle, strPrint.c_str(), strPrint.length(), &writed, NULL);
}

void CreateList(std::string path_name, size_t count)
{
	HANDLE handle = CreateFileA(path_name.c_str(), 
		GENERIC_WRITE, 
		FILE_SHARE_READ, 
		NULL, 
		OPEN_ALWAYS, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);

	if(handle == NULL)
		return;

	string guid;
	md5_engine md5e;
	BYTE byMD5[16] = {0};
	CHAR szMd5Value[33] = {0};

	size_t insert = 0;
	while(insert < count)
	{
		insert++;
		if(insert%10000 == 0)
			printf("%d\n", insert);

		guid = newGUID();
		md5e.digest(guid.c_str(), guid.length(),byMD5);
		::_snprintf_s(szMd5Value, 33, _TRUNCATE,
			"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
			byMD5[0], byMD5[1], byMD5[2], byMD5[3],
			byMD5[4], byMD5[5], byMD5[6], byMD5[7], 
			byMD5[8], byMD5[9], byMD5[10], byMD5[11],
			byMD5[12], byMD5[13], byMD5[14],byMD5[15]
		);
		write(handle, szMd5Value);
	}

	CloseHandle(handle);
}


int main () 
{
	CoInitialize(NULL);

	CreateList(white_list, 10000000);
	CreateList(black_list, 10000000);
	CreateList(gray_list,  800000000);

	CoUninitialize();
	return 0;
}

#include <ObjBase.h>
#include "md5_engine.h"
#include "OfflineCloud.h"

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

string GetMd5()
{
	string guid;
	md5_engine md5e;
	BYTE byMD5[16] = {0};
	CHAR szMd5Value[33] = {0};
	guid = newGUID();
	md5e.digest(guid.c_str(), guid.length(),byMD5);
	::_snprintf_s(szMd5Value, 33, _TRUNCATE,
		"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		byMD5[0], byMD5[1], byMD5[2], byMD5[3],
		byMD5[4], byMD5[5], byMD5[6], byMD5[7], 
		byMD5[8], byMD5[9], byMD5[10], byMD5[11],
		byMD5[12], byMD5[13], byMD5[14],byMD5[15]
	);
	return szMd5Value;
}

int main () 
{
	CoInitialize(NULL);

	int black_black = 0;
	int black_white = 0;
	int black_unknow = 0;
	int white_black = 0;
	int white_unknow = 0;
	int white_white = 0;
	int unknow_black = 0;
	int unknow_white = 0;
	int unknow_unknow = 0;

	int black = 0;
	int white = 0;
	int unknow = 0;
	COfflineCloud oc;
	if(oc.Init("off_black.dat", "off_black_v.dat", 
			"off_white.dat", "off_white_v.dat",
			"rt_black.dat", "rt_white.dat"))
	{
		//{
		//	ifstream fi("K:\\bloomfilter\\boost_bloom_filters\\black_list.log");  
		//	string s;
		//	size_t query = 0;
		//	while( getline(fi,s) )
		//	{    
		//		query++;
		//		if(query%10000 == 0)
		//			printf("%d\n", query);
		//		QUERY_RESULT result = oc.Query(s);
		//		if(result == em_white)
		//		{
		//			black_white++;
		//			//printf("%s em_white\n", s.c_str());
		//		}
		//		else if(result == em_black)
		//		{
		//			black_black++;
		//			//printf("%s em_black\n", s.c_str());
		//		}
		//		else
		//		{
		//			black_unknow++;
		//			//printf("%s em_unknown\n", s.c_str());
		//		}
		//	}
		//}
		//printf("black_white = %d\n",black_white);
		//printf("black_unknow = %d\n",black_unknow);
		//printf("black_black = %d\n",black_black);
		//{
		//	ifstream fi("K:\\bloomfilter\\boost_bloom_filters\\white_list.log");  
		//	string s;
		//	size_t query = 0;
		//	while( getline(fi,s) )
		//	{    
		//		query++;
		//		if(query%10000 == 0)
		//			printf("%d\n", query);
		//		QUERY_RESULT result = oc.Query(s);
		//		if(result == em_white)
		//		{
		//			white_white++;
		//			//printf("%s em_white\n", s.c_str());
		//		}
		//		else if(result == em_black)
		//		{
		//			white_black++;
		//			//printf("%s em_black\n", s.c_str());
		//		}
		//		else
		//		{
		//			white_unknow++;
		//			//printf("%s em_unknown\n", s.c_str());
		//		}
		//	}
		//}
		//printf("white_black = %d\n",white_black);
		//printf("white_unknow = %d\n",white_unknow);
		//printf("white_white = %d\n",white_white);
		//{
		//	ifstream fi("K:\\bloomfilter\\boost_bloom_filters\\gray_list.log");  
		//	string s;
		//	size_t query = 0;
		//	while( getline(fi,s) )
		//	{    
		//		query++;
		//		if(query%10000 == 0)
		//			printf("%d\n", query);
		//		QUERY_RESULT result = oc.Query(s);
		//		if(result == em_white)
		//		{
		//			unknow_white++;
		//			//printf("%s em_white\n", s.c_str());
		//		}
		//		else if(result == em_black)
		//		{
		//			unknow_black++;
		//			//printf("%s em_black\n", s.c_str());
		//		}
		//		else
		//		{
		//			unknow_unknow++;
		//			//printf("%s em_unknown\n", s.c_str());
		//		}
		//	}
		//}
		//printf("unknow_unknow = %d\n",unknow_unknow);
		//printf("unknow_black = %d\n",unknow_black);
		//printf("unknow_white = %d\n",unknow_white);
		int query = 0;
		while(true)
		{	
			query++;
			if(query % 500000 == 0)
			{
				printf("white = %d\n",white);
				printf("black = %d\n",black);
				printf("unknow = %d\n",unknow);
			}

			QUERY_RESULT result = oc.Query(GetMd5());
			if(result == em_white)
			{
				white++;
			}
			else if(result == em_black)
			{
				black++;
			}
			else
			{
				unknow++;
			}
		}


	}
	oc.Uninit();

	CoUninitialize();
	return 0;
}
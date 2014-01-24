#pragma once
#include <fstream>
#include <string>
using namespace std;


#define SET_BIT_8   12

typedef enum 
{
	em_unknown = 0,
	em_white,
	em_black
} QUERY_RESULT;

class COfflineCloud
{
public:
	COfflineCloud();
	virtual ~COfflineCloud();
	virtual bool Init(const string &db_black, const string &db_black_v,
					const string &db_white, const string &db_white_v,
					const string &rt_db_black, const string &rt_db_white);
	virtual void Uninit();

	virtual QUERY_RESULT Query(const string &md5);
	virtual void Insert(const string &md5, QUERY_RESULT &result);
	virtual void Reload();
private:
	bool CheckDb();
	bool LoadDb();
	void UnLoadDb();
	bool LoadRealTimeDb();
	void UnLoadRealTimeDb();
	string db_black_;
	string db_black_v_;
	string db_white_;
	string db_white_v_;
	string rt_db_black_;
	string rt_db_white_;

	void *p_bf_black_;
	void *p_bf_black_v_;
	void *p_bf_white_;
	void *p_bf_white_v_;
	void *p_rt_black_;
	void *p_rt_white_;
};
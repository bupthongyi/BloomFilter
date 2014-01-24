#pragma once

#include <fstream>
#include <string>
using namespace std;

#define SET_BIT_8   12

struct DB_GEN_PRAMA
{
	size_t black_bits_num;
	size_t black_verify_bits_num;
	size_t white_bits_num;
	size_t white_verify_bits_num;

	std::string black_db;
	std::string black_verify_db;
	std::string white_db;
	std::string white_verify_db;

	std::string black_record;
	std::string gray_record;
	std::string white_record;
};

class CDataBaseGen
{
public:
	CDataBaseGen();
	virtual ~CDataBaseGen();
	void Init(const DB_GEN_PRAMA &prama);
	void Uninit();

	void CreateBlackDb();
	void CreateWhiteDb();
	void CreateBlackVDb();
	void CreateWhiteVDb();

	size_t black_bits_num_;
	size_t black_verify_bits_num_;
	size_t white_bits_num_;
	size_t white_verify_bits_num_;

	std::string black_db_;
	std::string black_verify_db_;
	std::string white_db_;
	std::string white_verify_db_;

	std::string black_record_;
	std::string gray_record_;
	std::string white_record_;
};
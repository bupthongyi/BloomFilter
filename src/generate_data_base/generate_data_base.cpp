#include "DataBaseGen.h"

int main () 
{
	DB_GEN_PRAMA prama;
	prama.black_bits_num			= 40*1024*8*1024;
	prama.black_verify_bits_num		= 5*1024*8*1024;
	prama.white_bits_num			= 40*1024*8*1024;
	prama.white_verify_bits_num		= 5*1024*8*1024;

	prama.black_db			= "off_black.dat";
	prama.black_verify_db	= "off_black_v.dat";
	prama.white_db			= "off_white.dat";
	prama.white_verify_db	= "off_white_v.dat";

	prama.black_record		= "black_list.log";
	prama.gray_record		= "gray_list.log";;
	prama.white_record		= "white_list.log";;

	CDataBaseGen dbGen;
	dbGen.Init(prama);
	dbGen.CreateBlackDb();
	dbGen.CreateWhiteDb();
	dbGen.CreateBlackVDb();
	dbGen.CreateWhiteVDb();
	dbGen.Uninit();
	return 0;
}
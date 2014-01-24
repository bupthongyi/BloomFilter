#include "DataBaseGen.h"
#include <boost/bloom_filter/twohash_dynamic_basic_bloom_filter.hpp>
#include <boost/dynamic_bitset.hpp>

using boost::bloom_filters::twohash_dynamic_basic_bloom_filter;
using boost::bloom_filters::boost_hash;
using boost::bloom_filters::murmurhash3;
using boost::bloom_filters::detail::cube;
using boost::bloom_filters::detail::incompatible_size_exception;

CDataBaseGen::CDataBaseGen()
{
}

CDataBaseGen::~CDataBaseGen()
{
}

void CDataBaseGen::Init(const DB_GEN_PRAMA &prama)
{
	black_bits_num_ = prama.black_bits_num;
	black_verify_bits_num_ = prama.black_verify_bits_num;
	white_bits_num_ = prama.white_bits_num;
	white_verify_bits_num_ = prama.white_verify_bits_num;

	black_db_ = prama.black_db;
	black_verify_db_ = prama.black_verify_db;
	white_db_ = prama.white_db;
	white_verify_db_ = prama.white_verify_db;

	black_record_ = prama.black_record;
	gray_record_ = prama.gray_record;
	white_record_ = prama.white_record;
}

void CDataBaseGen::Uninit()
{

}

void CDataBaseGen::CreateBlackDb()
{
	ifstream fi(black_record_.c_str());  
	ofstream fo(black_db_.c_str(), ios_base::out | ios_base::binary);

	twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> > *p_bf;

	p_bf = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> >(black_bits_num_);

	string s;
	size_t insert = 0;
	while( getline(fi,s) )
	{
		if(32 != s.length())
			continue;

		insert++;
		if(insert%10000 == 0)
			printf("%d\n", insert);
		p_bf->insert(s);
	}

	//写入文件
	size_t nBlockCount = p_bf->bits.num_blocks();
	boost::dynamic_bitset<>::block_type* blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
	to_block_range(p_bf->bits, blocks);
	fo.write((char*)blocks, nBlockCount * sizeof(*blocks));
	delete[] blocks;

	fi.close();
	fo.close();
}

void CDataBaseGen::CreateWhiteDb()
{
	ifstream fi(white_record_.c_str());  
	ofstream fo(white_db_.c_str(), ios_base::out | ios_base::binary);

	twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> > *p_bf;

	p_bf = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> >(white_bits_num_);

	string s;
	size_t insert = 0;
	while( getline(fi,s) )
	{
		if(32 != s.length())
			continue;

		insert++;
		if(insert%10000 == 0)
			printf("%d\n", insert);
		p_bf->insert(s);
	}

	//写入文件
	size_t nBlockCount = p_bf->bits.num_blocks();
	boost::dynamic_bitset<>::block_type* blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
	to_block_range(p_bf->bits, blocks);
	fo.write((char*)blocks, nBlockCount * sizeof(*blocks));
	delete[] blocks;

	fi.close();
	fo.close();
}

void CDataBaseGen::CreateBlackVDb()
{
	ifstream fi(black_db_.c_str(), ios_base::in | ios_base::binary);
	ifstream f_gray(gray_record_.c_str());
	ifstream f_white(white_record_.c_str());
	ofstream fo(black_verify_db_.c_str(), ios_base::out | ios_base::binary);

	twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> > *p_bf;
	p_bf = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> >(black_bits_num_);

	twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 67>, murmurhash3<string, 2711> > *p_bf_v;
	p_bf_v = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 67>, murmurhash3<string, 2711> >(black_verify_bits_num_);
	
	size_t nBlockCount = p_bf->bits.num_blocks();
	boost::dynamic_bitset<>::block_type* newBlocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
	fi.read((char*)newBlocks, nBlockCount * sizeof(*newBlocks));
	fi.close();
	from_block_range(newBlocks, newBlocks + nBlockCount, p_bf->bits);
	delete[] newBlocks;
	
	string s;
	size_t collisions = 0;
	size_t query = 0;
	while( getline(f_gray,s) )
	{
		if(32 != s.length())
			continue;

		query++;
		if(query%10000 == 0)
			printf("%d\n", query);
		if (p_bf->probably_contains(s)) 
		{
			p_bf_v->insert(s);
			++collisions;
		}
	}
	
	collisions = 0;
	query = 0;
	while( getline(f_white,s) )
	{
		if(32 != s.length())
			continue;

		query++;
		if(query%10000 == 0)
			printf("%d\n", query);
		if (p_bf->probably_contains(s)) 
		{
			p_bf_v->insert(s);
			++collisions;
		}
	}
	
	nBlockCount = p_bf_v->bits.num_blocks();
	boost::dynamic_bitset<>::block_type* blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
	to_block_range(p_bf_v->bits, blocks);
	fo.write((char*)blocks, nBlockCount * sizeof(*blocks));
	delete[] blocks;

	fi.close();
	f_gray.close();
	f_white.close();
	fo.close();
}

void CDataBaseGen::CreateWhiteVDb()
{

	ifstream fi(white_db_.c_str(), ios_base::in | ios_base::binary);
	ifstream f_gray(gray_record_.c_str());
	ifstream f_black(black_record_.c_str());
	ofstream fo(white_verify_db_.c_str(), ios_base::out | ios_base::binary);

	twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> > *p_bf;
	p_bf = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> >(white_bits_num_);

	twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 67>, murmurhash3<string, 2711> > *p_bf_v;
	p_bf_v = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 67>, murmurhash3<string, 2711> >(white_verify_bits_num_);

	size_t nBlockCount = p_bf->bits.num_blocks();
	boost::dynamic_bitset<>::block_type* newBlocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
	fi.read((char*)newBlocks, nBlockCount * sizeof(*newBlocks));
	fi.close();
	from_block_range(newBlocks, newBlocks + nBlockCount, p_bf->bits);
	delete[] newBlocks;

	string s;
	size_t collisions = 0;
	size_t query = 0;
	while( getline(f_gray,s) )
	{
		if(32 != s.length())
			continue;

		query++;
		if(query%10000 == 0)
			printf("%d\n", query);
		if (p_bf->probably_contains(s)) 
		{
			p_bf_v->insert(s);
			++collisions;
		}
	}

	collisions = 0;
	query = 0;
	while( getline(f_black,s) )
	{
		if(32 != s.length())
			continue;

		query++;
		if(query%10000 == 0)
			printf("%d\n", query);
		if (p_bf->probably_contains(s)) 
		{
			p_bf_v->insert(s);
			++collisions;
		}
	}

	nBlockCount = p_bf_v->bits.num_blocks();
	boost::dynamic_bitset<>::block_type* blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
	to_block_range(p_bf_v->bits, blocks);
	fo.write((char*)blocks, nBlockCount * sizeof(*blocks));
	delete[] blocks;

	fi.close();
	f_gray.close();
	f_black.close();
	fo.close();
}
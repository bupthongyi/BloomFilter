#include "OfflineCloud.h"
#include <Shlwapi.h>

#include <boost/bloom_filter/twohash_dynamic_basic_bloom_filter.hpp>
#include <boost/dynamic_bitset.hpp>

using boost::bloom_filters::twohash_dynamic_basic_bloom_filter;
using boost::bloom_filters::boost_hash;
using boost::bloom_filters::murmurhash3;
using boost::bloom_filters::detail::cube;
using boost::bloom_filters::detail::incompatible_size_exception;


COfflineCloud::COfflineCloud()
	: p_bf_black_(NULL)
	, p_bf_black_v_(NULL)
	, p_bf_white_(NULL)
	, p_bf_white_v_(NULL)
	, p_rt_black_(NULL)
	, p_rt_white_(NULL)
{
}

COfflineCloud::~COfflineCloud()
{
}

bool COfflineCloud::Init(const string &db_black, const string &db_black_v,
						 const string &db_white, const string &db_white_v,
						 const string &rt_db_black, const string &rt_db_white)
{
	db_black_	= db_black;
	db_white_	= db_white;
	db_black_v_ = db_black_v;
	db_white_v_ = db_white_v;
	rt_db_black_ = rt_db_black;
	rt_db_white_ = rt_db_white;

	if(!CheckDb())
		return false;

	if(!LoadDb())
		return false;

	if(!LoadRealTimeDb())
		return false;

	return true;
}

void COfflineCloud::Uninit()
{
	UnLoadRealTimeDb();
	UnLoadDb();
}

QUERY_RESULT COfflineCloud::Query(const string &md5)
{
	//// 在容忍误报率的情况下先查询黑
	if(((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_black_)->probably_contains(md5))
	{
		if(!((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 67>, murmurhash3<string, 2711> >*)p_bf_black_v_)->probably_contains(md5))
		{
			return em_black;
		}
	}
	if(((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> >*)p_bf_black_)->probably_contains(md5))
	{
		if(!(((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 67>, murmurhash3<string, 2711> >*)p_bf_black_v_)->probably_contains(md5)))
		{
			return em_black;
		}
	}
	// 再查询白
	if(((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_white_)->probably_contains(md5))
	{
		if(!(((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 67>, murmurhash3<string, 2711> >*)p_bf_white_v_)->probably_contains(md5)))
		{
			return em_white;
		}
	}
	if(((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 1471>, murmurhash3<string, 3001> >*)p_bf_white_)->probably_contains(md5))
	{
		if(!(((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 67>, murmurhash3<string, 2711> >*)p_bf_white_v_)->probably_contains(md5)))
		{
			return em_white;
		}
	}
	return em_unknown;
}

void COfflineCloud::Insert(const string &md5, QUERY_RESULT &result)
{
}

void COfflineCloud::Reload()
{
}

bool COfflineCloud::CheckDb()
{
	if(!PathFileExistsA(db_black_.c_str()) || 
		!PathFileExistsA(db_black_v_.c_str()) ||
		!PathFileExistsA(db_white_.c_str()) ||
		!PathFileExistsA(db_white_v_.c_str()))
		return false;

	WIN32_FILE_ATTRIBUTE_DATA stFileInfo = {0};
	if(0 != GetFileAttributesExA(db_black_.c_str(),GetFileExInfoStandard,&stFileInfo))
	{
		if(40*1024*1024 != stFileInfo.nFileSizeLow)
			return false;
	}
	if(0 != GetFileAttributesExA(db_white_.c_str(),GetFileExInfoStandard,&stFileInfo))
	{
		if(40*1024*1024 != stFileInfo.nFileSizeLow)
			return false;
	}
	if(0 != GetFileAttributesExA(db_black_v_.c_str(),GetFileExInfoStandard,&stFileInfo))
	{
		if(5*1024*1024 != stFileInfo.nFileSizeLow)
			return false;
	}
	if(0 != GetFileAttributesExA(db_white_v_.c_str(),GetFileExInfoStandard,&stFileInfo))
	{
		if(5*1024*1024 != stFileInfo.nFileSizeLow)
			return false;
	}
	return true;
}

bool COfflineCloud::LoadDb()
{
	bool result = true;
	// black db
	{
		p_bf_black_ = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 1471>, murmurhash3<string, 3001> >(40*1024*8*1024);

		size_t nBlockCount = ((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 1471>, murmurhash3<string, 3001> >*)p_bf_black_)->bits.num_blocks();
		ifstream fi_black(db_black_.c_str(), ios_base::in | ios_base::binary);
		boost::dynamic_bitset<>::block_type* black_blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
		if(black_blocks && fi_black.read((char*)black_blocks, nBlockCount*sizeof(*black_blocks)))
		{
			boost::from_block_range(black_blocks, black_blocks + nBlockCount, 
				((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
				murmurhash3<string, 1471>, murmurhash3<string, 3001> >*)p_bf_black_)->bits);
		}
		else
		{
			result = false;
		}
		fi_black.close();
		if(black_blocks)
			delete[] black_blocks;
	}
	
	// black verify db
	{
		p_bf_black_v_ = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 67>, murmurhash3<string, 2711> >(5*1024*8*1024);

		size_t nBlockCount = ((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 67>, murmurhash3<string, 2711> >*)p_bf_black_v_)->bits.num_blocks();
		ifstream fi_black_v(db_black_v_.c_str(), ios_base::in | ios_base::binary);
		boost::dynamic_bitset<>::block_type* black_v_blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
		if(black_v_blocks && fi_black_v.read((char*)black_v_blocks, nBlockCount*sizeof(*black_v_blocks)))
		{
			boost::from_block_range(black_v_blocks, black_v_blocks + nBlockCount, 
				((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
				murmurhash3<string, 67>, murmurhash3<string, 2711> >*)p_bf_black_v_)->bits);
		}
		else
		{
			result = false;
		}
		fi_black_v.close();
		if(black_v_blocks)
			delete[] black_v_blocks;
	}

	// white db
	{
		p_bf_white_ = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 1471>, murmurhash3<string, 3001> >(40*1024*8*1024);

		size_t nBlockCount = ((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 1471>, murmurhash3<string, 3001> >*)p_bf_white_)->bits.num_blocks();
		ifstream fi_white(db_white_.c_str(), ios_base::in | ios_base::binary);
		boost::dynamic_bitset<>::block_type* white_blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
		if(white_blocks && fi_white.read((char*)white_blocks, nBlockCount*sizeof(*white_blocks)))
		{
			boost::from_block_range(white_blocks, white_blocks + nBlockCount, 
				((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
				murmurhash3<string, 1471>, murmurhash3<string, 3001> >*)p_bf_white_)->bits);
		}
		else
		{
			result = false;
		}
		fi_white.close();
		if(white_blocks)
			delete[] white_blocks;
	}
	
	// white verify db
	{
		p_bf_white_v_ = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 67>, murmurhash3<string, 2711> >(5*1024*8*1024);

		size_t nBlockCount = ((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 67>, murmurhash3<string, 2711> >*)p_bf_white_v_)->bits.num_blocks();
		ifstream fi_white_v(db_white_v_.c_str(), ios_base::in | ios_base::binary);
		boost::dynamic_bitset<>::block_type* white_v_blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
		if(white_v_blocks && fi_white_v.read((char*)white_v_blocks, nBlockCount*sizeof(*white_v_blocks)))
		{
			boost::from_block_range(white_v_blocks, white_v_blocks + nBlockCount, 
				((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
				murmurhash3<string, 67>, murmurhash3<string, 2711> >*)p_bf_white_v_)->bits);
		}
		else
		{
			result = false;
		}
		fi_white_v.close();
		if(white_v_blocks)
			delete[] white_v_blocks;
	}

	return result;
}


void COfflineCloud::UnLoadDb()
{

}

bool COfflineCloud::LoadRealTimeDb()
{
	p_rt_black_ = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 33>, murmurhash3<string, 1733> >(5*1024*8*1024);

	if(PathFileExistsA(rt_db_black_.c_str()))
	{
		WIN32_FILE_ATTRIBUTE_DATA stFileInfo = {0};
		if(0 != GetFileAttributesExA(rt_db_black_.c_str(),GetFileExInfoStandard,&stFileInfo))
		{
			if(5*1024*1024 == stFileInfo.nFileSizeLow)
			{
				size_t nBlockCount = ((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
					murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_black_)->bits.num_blocks();
				ifstream fi(rt_db_black_.c_str(), ios_base::in | ios_base::binary);
				boost::dynamic_bitset<>::block_type* blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
				if(blocks && fi.read((char*)blocks, nBlockCount*sizeof(*blocks)))
				{
					boost::from_block_range(blocks, blocks + nBlockCount, 
						((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
						murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_black_)->bits);
				}
				fi.close();
				if(blocks)
					delete[] blocks;
			}
		}
		
	}

	p_rt_white_ = new twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
		murmurhash3<string, 33>, murmurhash3<string, 1733> >(5*1024*8*1024);

	if(PathFileExistsA(rt_db_white_.c_str()))
	{
		WIN32_FILE_ATTRIBUTE_DATA stFileInfo = {0};
		if(0 != GetFileAttributesExA(rt_db_white_.c_str(),GetFileExInfoStandard,&stFileInfo))
		{
			if(5*1024*1024 == stFileInfo.nFileSizeLow)
			{
				size_t nBlockCount = ((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
					murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_white_)->bits.num_blocks();
				ifstream fi(rt_db_white_.c_str(), ios_base::in | ios_base::binary);
				boost::dynamic_bitset<>::block_type* blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
				if(blocks && fi.read((char*)blocks, nBlockCount*sizeof(*blocks)))
				{
					boost::from_block_range(blocks, blocks + nBlockCount, 
						((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
						murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_white_)->bits);
				}
				fi.close();
				if(blocks)
					delete[] blocks;
			}
		}

	}

	return true;
}

void COfflineCloud::UnLoadRealTimeDb()
{
	{
		ofstream fo(rt_db_black_.c_str(), ios_base::out | ios_base::binary);
		size_t nBlockCount = ((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_black_)->bits.num_blocks();
		boost::dynamic_bitset<>::block_type* blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
		boost::to_block_range(((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_black_)->bits, blocks);
		fo.write((char*)blocks, nBlockCount * sizeof(*blocks));
		delete[] blocks;
	}
	{
		ofstream fo(rt_db_white_.c_str(), ios_base::out | ios_base::binary);
		size_t nBlockCount = ((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_white_)->bits.num_blocks();
		boost::dynamic_bitset<>::block_type* blocks = new boost::dynamic_bitset<>::block_type[nBlockCount];
		boost::to_block_range(((twohash_dynamic_basic_bloom_filter<string, SET_BIT_8, 0, 
			murmurhash3<string, 33>, murmurhash3<string, 1733> >*)p_rt_white_)->bits, blocks);
		fo.write((char*)blocks, nBlockCount * sizeof(*blocks));
		delete[] blocks;
	}
}
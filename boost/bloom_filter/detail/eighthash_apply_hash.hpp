//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Alejandro Cabrera 2011.
// Distributed under the Boost
// Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or
// copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/bloom_filter for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_BLOOM_FILTER_TWOHASH_APPLY_HASH_HPP
#define BOOST_BLOOM_FILTER_TWOHASH_APPLY_HASH_HPP

namespace boost {
namespace bloom_filters {
namespace detail {

	template <size_t N, typename Container>
	struct eighthash_apply_hash
	{
	private:
		typedef typename Container::value_type value_type;
		typedef typename Container::bitset_type bitset_type;
		typedef typename Container::hash_function1_type hash_function1_type;
		typedef typename Container::hash_function2_type hash_function2_type;
		typedef typename Container::hash_function3_type hash_function3_type;
		typedef typename Container::hash_function4_type hash_function4_type;
		typedef typename Container::hash_function5_type hash_function5_type;
		typedef typename Container::hash_function6_type hash_function6_type;
		typedef typename Container::hash_function7_type hash_function7_type;
		typedef typename Container::hash_function8_type hash_function8_type;
		typedef typename Container::extension_function_type extension_function_type;

	public:
        static void insert(const value_type& t, bitset_type& bits) 
		{
			static hash_function1_type hasher1;
			static hash_function2_type hasher2;
			static hash_function3_type hasher3;
			static hash_function4_type hasher4;
			static hash_function5_type hasher5;
			static hash_function6_type hasher6;
			static hash_function7_type hasher7;
			static hash_function8_type hasher8;
			static extension_function_type extender;

			const size_t hash1 = hasher1(t);
			const size_t hash2 = hasher2(t);
			const size_t hash3 = hasher3(t);
			const size_t hash4 = hasher4(t);
			const size_t hash5 = hasher5(t);
			const size_t hash6 = hasher6(t);
			const size_t hash7 = hasher7(t);
			const size_t hash8 = hasher8(t);


			//for (size_t i = 0; i < N; ++i) 
			{
				//const size_t hash_val = hash1 + i * hash2 + extender(i);
				bits[hash1 % bits.size()] = true;
				bits[hash2 % bits.size()] = true;
				bits[hash3 % bits.size()] = true;
				bits[hash4 % bits.size()] = true;
				bits[hash5 % bits.size()] = true;
				bits[hash6 % bits.size()] = true;
				bits[hash7 % bits.size()] = true;
				bits[hash8 % bits.size()] = true;
			}
		}

		static bool contains(const value_type& t, const bitset_type& bits)
		{
			static hash_function1_type hasher1;
			static hash_function2_type hasher2;
			static hash_function3_type hasher3;
			static hash_function4_type hasher4;
			static hash_function5_type hasher5;
			static hash_function6_type hasher6;
			static hash_function7_type hasher7;
			static hash_function8_type hasher8;
			static extension_function_type extender;

			const size_t hash1 = hasher1(t);
			const size_t hash2 = hasher2(t);
			const size_t hash3 = hasher3(t);
			const size_t hash4 = hasher4(t);
			const size_t hash5 = hasher5(t);
			const size_t hash6 = hasher6(t);
			const size_t hash7 = hasher7(t);
			const size_t hash8 = hasher8(t);

			//for (size_t i = 0; i < N; ++i) 
			{
				//const size_t hash_val = hash1 + i * hash2 + extender(i);
				if (bits[hash1 % bits.size()] != true || 
					bits[hash2 % bits.size()] != true || 
					bits[hash3 % bits.size()] != true || 
					bits[hash4 % bits.size()] != true || 
					bits[hash5 % bits.size()] != true || 
					bits[hash6 % bits.size()] != true || 
					bits[hash7 % bits.size()] != true || 
					bits[hash8 % bits.size()] != true)
					return false;
			}

			return true;
		}
	};

} // namespace detail
} // namespace bloom_filter
} // namespace boost
#endif

//
// Created by Li Shuang ( pseudonym ) on 2024-07-10 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#ifndef PG_SIZE
#define PG_SIZE (4*_1K)
#endif

#ifndef PN_MASK
#define PN_MASK 0x1FFUL
#endif

#define PT_LEVEL 4

namespace hsai
{
	class PageTable;

	/// @brief 页表项，实际上是页表项的指针
	class Pte __hsai_hal
	{
		friend PageTable;
	private:
		pte_t *_data_addr = nullptr;
	public:
		Pte() = default;
		Pte( pte_t *addr );
		void set_addr( pte_t *addr ) { _data_addr = addr; }
		bool is_null() { return _data_addr == nullptr; }

		bool is_valid();
		bool is_dirty();
		bool is_present();
		bool is_writable();
		bool is_readable();
		bool is_executable();
		bool is_dir_page();
		bool is_super_plv();
		bool is_user_plv();

		void set_user_plv();
		void set_super_plv();

		ulong to_pa();
		pte_t get_flags();

		static pte_t map_code_page_flags();
		static pte_t map_data_page_flags();
		static pte_t map_dir_page_flags();
		static pte_t super_plv_flag();
		static pte_t user_plv_flag();
		static pte_t valid_flag();
		// uint64 plv() { return ( ( *_data_addr & loongarch::PteEnum::plv_m ) >> loongarch::PteEnum::plv_s ); }
		// uint64 mat() { return ( ( *_data_addr & loongarch::PteEnum::mat_m ) >> loongarch::PteEnum::mat_s ); }
		// uint64 flags() { return *_data_addr & loongarch::PteEnum::b_flags_m; }
		// void set_plv() { *_data_addr |= loongarch::PteEnum::plv_m; }
		// void unset_plv() { *_data_addr &= ~loongarch::PteEnum::plv_m; }
		// bool is_global() { return ( ( *_data_addr & loongarch::PteEnum::b_global_m ) != 0 ); }
		// bool is_restrict_plv() { return ( ( *_data_addr & loongarch::PteEnum::rplv_m ) != 0 ); }
		// bool is_leaf() { return ( ( *_data_addr & loongarch::qemuls2k::ls2kPgEnum::pg_flags_mask ) != 1 ); }


		// 慎用！！！这个函数会使PTE的值清零！
		void clear_data() { *_data_addr = 0; }

		pte_t get_data() { return *_data_addr; }
		void set_data( uint64 data ) { *_data_addr = data; }
	};

	constexpr ulong page_size = PG_SIZE;
	static_assert( ( page_size & ( page_size - 1 ) ) == 0 );
	constexpr ulong page_size_shift = [] () -> ulong
	{
		ulong i = 1;
		ulong k = 0;
		for ( ; ( i & page_size ) == 0; i <<= 1, ++k );
		return k;
	}( );

	constexpr ulong page_number_mask = PN_MASK;
	constexpr ulong page_number_mask_width = [] ()->ulong
	{
		ulong i = 1;
		ulong k = 0;
		for ( ; ( i & page_number_mask ) != 0; i <<= 1, ++k );
		return k;
	}( );

	constexpr ulong page_round_up( ulong addr )
	{
		ulong pg_sz = page_size;
		return ( ( addr + pg_sz - 1 ) & ~( pg_sz - 1 ) );
	}

	constexpr ulong page_round_down( ulong addr )
	{
		ulong pg_sz = page_size;
		return ( addr & ~( pg_sz - 1 ) );
	}

	constexpr bool is_page_align( uint64 addr )
	{
		ulong pg_sz = page_size;
		return ( addr & ( pg_sz - 1 ) ) == 0;
	}

#if PT_LEVEL == 4
	constexpr ulong pgd_mask = page_number_mask << page_size_shift << ( page_number_mask_width * 3 );
	constexpr ulong pud_mask = page_number_mask << page_size_shift << ( page_number_mask_width * 2 );
	constexpr ulong pmd_mask = page_number_mask << page_size_shift << ( page_number_mask_width * 1 );
	constexpr ulong pt_mask = page_number_mask << page_size_shift << ( page_number_mask_width * 0 );
#endif
#if PT_LEVEL == 3
	constexpr ulong pgd_mask = page_number_mask << page_size_shift << ( page_number_mask_width * 2 );
	constexpr ulong pud_mask = 0;
	constexpr ulong pmd_mask = page_number_mask << page_size_shift << ( page_number_mask_width * 1 );
	constexpr ulong pt_mask = page_number_mask << page_size_shift << ( page_number_mask_width * 0 );
#endif
#if PT_LEVEL == 2
	constexpr ulong pgd_mask = page_number_mask << page_size_shift << ( page_number_mask_width * 1 );
	constexpr ulong pud_mask = 0;
	constexpr ulong pmd_mask = 0;
	constexpr ulong pt_mask = page_number_mask << page_size_shift << ( page_number_mask_width * 0 );
#endif

#if PT_LEVEL > 4 || PT_LEVEL < 2
#error "不支持的页表级别"
#endif

	constexpr ulong lowest_1_bit( ulong i )
	{
		ulong k = 1;
		ulong cnt = 0;
		for ( ; ( k & i ) == 0; k <<= 1, cnt++ );
		return cnt;
	}

	constexpr ulong pgd_mask_shift = pgd_mask == 0 ? 0 : lowest_1_bit( pgd_mask );
	constexpr ulong pud_mask_shift = pud_mask == 0 ? 0 : lowest_1_bit( pud_mask );
	constexpr ulong pmd_mask_shift = pmd_mask == 0 ? 0 : lowest_1_bit( pmd_mask );
	constexpr ulong pt_mask_shift = pt_mask == 0 ? 0 : lowest_1_bit( pt_mask );

	constexpr ulong pgd_num( ulong va )
	{
		return ( pgd_mask == 0 ) ? -1 : ( ( va & pgd_mask ) >> pgd_mask_shift );
	}

	constexpr ulong pud_num( ulong va )
	{
		return ( pud_mask == 0 ) ? -1 : ( ( va & pud_mask ) >> pud_mask_shift );
	}

	constexpr ulong pmd_num( ulong va )
	{
		return ( pmd_mask == 0 ) ? -1 : ( ( va & pmd_mask ) >> pmd_mask_shift );
	}

	constexpr ulong pt_num( ulong va )
	{
		return ( pt_mask == 0 ) ? -1 : ( ( va & pt_mask ) >> pt_mask_shift );
	}

} // namespace hsai

//
// Created by Li Shuang ( pseudonym ) on 2024-08-18
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use
// | Contact Author: lishuang.mk@whu.edu.cn
// --------------------------------------------------------------
//

#include "klib/back_trace.hh"

#include <hsai_global.hh>

#include "klib/klib.hh"
#include "mm/memlayout.hh"

extern "C" char	 entry_stack[];
extern "C" char *entry_stack_end;

namespace klib
{

#ifdef __gnu_linux__

	bool sp_is_in_kstack( void *sp )
	{
		u64 st	= mm::vml::vm_kernel_start;
		u64 ed	= mm::vml::vm_kernel_end;
		u64 spa = (u64) sp;
		return spa >= st && spa < ed;

		// char *spa = (char *) sp;
		// return spa >= entry_stack && spa < entry_stack_end;
	}

	int back_trace_fp( void **ptr_buf, int ptr_max_cnt )
	{
		int	 cnt = 0;
		u64 *fp	 = (u64 *) __builtin_frame_address( 0 );
		while ( fp != nullptr )
		{
			ptr_buf[cnt] = fp;
			cnt++;
			if ( cnt >= ptr_max_cnt ) break;

			u64 *nfp1, *nfp2;

			nfp1 = (u64 *) *( fp - 1 );
			nfp2 = (u64 *) *( fp - 2 );

			if ( sp_is_in_kstack( nfp1 ) )
			{
				fp = nfp1;
				continue;
			}
			else if ( sp_is_in_kstack( nfp2 ) )
			{
				fp = nfp2;
				continue;
			}
			else
			{
				if ( nfp1 == nullptr || nfp2 == nullptr ) break;
				log_error( "can't resolve fp" );
				while ( 1 );
			}
		}
		return cnt;
	}

	int back_trace_ra( void **ptr_buf, int ptr_max_cnt )
	{
		int	 cnt = 0;
		u64 *fp	 = (u64 *) __builtin_frame_address( 0 );
		u64 *ra	 = nullptr;
		while ( fp != nullptr )
		{
			cnt++;
			if ( cnt >= ptr_max_cnt ) break;

			u64 *nfp1, *nfp2;

			nfp1 = (u64 *) *( fp - 1 );
			nfp2 = (u64 *) *( fp - 2 );

			if ( sp_is_in_kstack( nfp1 ) )
			{
				fp = nfp1;
				ra = (u64 *) __builtin_return_address( 0 );
			}
			else if ( sp_is_in_kstack( nfp2 ) )
			{
				fp = nfp2;
				ra = nfp1;
			}
			else
			{
				if ( nfp1 == nullptr || nfp2 == nullptr ) break;
				log_error( "can't resolve fp" );
				while ( 1 );
			}

			ptr_buf[cnt] = ra;
		}
		return cnt;
	}

#else
#	error "back tace 只允许在gcc环境下使用"
#endif

} // namespace klib

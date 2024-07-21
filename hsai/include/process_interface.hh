//
// Created by Li Shuang ( pseudonym ) on 2024-07-11 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once

#include <kernel/types.hh>

namespace hsai
{
	class VirtualPageTable;

	constexpr uint proc_pool_size = PROC_POOL_SIZE;

	extern __hsai_kernel void * get_cur_proc();

	extern __hsai_kernel void * get_trap_frame_from_proc( void * proc );

	extern __hsai_kernel ulong get_trap_frame_vir_addr();

	extern __hsai_kernel uint get_pid( void * proc );

	extern __hsai_kernel void exit_proc( void * proc, int state );

	extern __hsai_kernel bool proc_is_killed( void * proc );

	extern __hsai_kernel bool proc_is_running( void * proc );

	extern __hsai_kernel void sched_proc( void * proc );

	extern __hsai_kernel ulong get_kstack_from_proc( void * proc );

	extern __hsai_kernel ulong get_pgd_addr( void * proc );

	extern __hsai_kernel VirtualPageTable * get_pt_from_proc( void * proc );


	extern __hsai_hal void user_proc_init( void * proc );

	extern __hsai_hal void set_trap_frame_return_value( void * trapframe, ulong value );

	extern __hsai_hal void set_trap_frame_user_sp( void * trapframe, ulong sp );

	extern __hsai_hal void set_trap_frame_arg( void * trapframe, uint arg_num, ulong value );

	extern __hsai_hal void copy_trap_frame( void * from, void * to );

	extern __hsai_hal ulong get_arg_from_trap_frame( void * trapframe, uint arg_num );

	extern __hsai_hal void user_trap_return();

	extern __hsai_hal const uint context_size;

	extern __hsai_hal void set_context_entry( void * cont, void * entry );

	extern __hsai_hal void set_context_sp( void * cont, ulong sp );

	extern __hsai_hal void * get_context_address( uint proc_gid );

} // namespace hsai

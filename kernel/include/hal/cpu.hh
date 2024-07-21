// //
// // Created by Li shuang ( pseudonym ) on 2024-03-27 
// // --------------------------------------------------------------
// // | Note: This code file just for study, not for commercial use 
// // | Contact Author: lishuang.mk@whu.edu.cn 
// // --------------------------------------------------------------
// //

// #pragma once

// #include "hal/loongarch.hh"
// #include "hal/csr.hh"

// #include "pm/context.hh"

// namespace pm
// {
// 	class Pcb;
// } // namespace pm


// namespace loongarch
// {
// 	class Cpu
// 	{
// 	private:
// 		pm::Pcb *_cur_proc;			// 当前进程
// 		pm::Context _context;			// 进程上下文
// 		int _num_off;				// 关闭中断层数
// 		int _int_ena;				// 关中断前中断开关状态

// 	public:
// 		pm::Context *get_context() { return &_context; }
// 		int get_num_off() { return _num_off; }
// 		int get_int_ena() { return _int_ena; }
// 		void set_int_ena( int x ) { _int_ena = x; }
// 		void set_cur_proc( pm::Pcb *p ) { _cur_proc = p; }

// 		// read stack pointer
// 		static inline uint64 read_sp()
// 		{
// 			uint64 x;
// 			asm volatile( "addi.d %0, $sp, 0" : "=r" ( x ) );
// 			return x;
// 		}

// 		// read thread pointer ( cpu id )
// 		static inline uint64 read_tp()
// 		{
// 			uint64 x;
// 			asm volatile( "addi.d %0, $tp, 0" : "=r" ( x ) );
// 			return x;
// 		}

// 		// read return address ( register ra )
// 		static inline uint64 read_ra()
// 		{
// 			uint64 x;
// 			asm volatile( "addi.d %0, $ra, 0" : "=r" ( x ) );
// 			return x;
// 		}

// 		/// @brief get current cpu info 
// 		/// @return cpu info 
// 		static Cpu *get_cpu();

// 		static inline int get_intr_stat(){
// 			uint32 x = read_csr( csr::CsrAddr::crmd );
// 			return ( x & csr::Crmd::ie_m ) != 0;
// 		};

// 		static void push_intr_off();
// 		static void pop_intr_off();
// 		static void euen_float();

// 		static void write_csr( csr::CsrAddr r, uint64 d );
// 		static uint64 read_csr( csr::CsrAddr r );

// 		static inline void interrupt_on() { return _intr_on(); }
// 		static inline void interrupt_off() { return _intr_off(); }
// 		pm::Pcb *get_cur_proc() { return _cur_proc; }
// 	private:
// 		static void _intr_on();
// 		static void _intr_off();
// 	};

// 	extern Cpu k_cpus[ NUMCPU ];

// 	// extern "C"{
// 	// 	void swtch(Context *old_, Context *new_);
// 	// }
// } // namespace loongarch 
//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "hal/ata/ata_cmd.hh"
#include "hal/sata/hba_port.hh"
#include "hal/sata/sata_driver.hh"

namespace ata
{
	namespace sata
	{
		struct HbaMemReg;
		struct HbaPortReg;
		struct HbaCmdHeader;

		struct HbaMemVendorRegLs2k
		{
			dword bistafr;
			dword bistcr;
			dword bistctr;
			dword bistsr;
			dword bistdecr;
			dword oobr;
			dword timer1ms;
			dword gparam1r;
			dword gparam2r;
			dword pparamr;
			dword testr;
			dword versionr;
			dword idr;
		}__attribute__( ( __packed__ ) );

		/// @brief 不明寄存器，迁移自rt-thread
		enum HbaLs2kOobr : uint32
		{
			hba_oobr_we = 0x80000000,
			hba_oobr_cwmin_m = 0x7f000000,
			hba_oobr_cwmax_m = 0x00ff0000,
			hba_oobr_cimin_m = 0x0000ff00,
			hba_oobr_cimax_m = 0x000000ff,
		};

		constexpr uint max_port_num = 32;

		class SataLs2k : public SataDriver
		{
		private:
			HbaMemReg *_hba_mem_reg;
			HbaMemVendorRegLs2k *_hba_vendor_reg;
			HbaPortReg *_hba_port_reg[ max_port_num ];
			uint _port_num = 0;
			uint32 _port_map = 0;
			uint32 _cap_cache = 0;

			void *_port_cmd_lst_base[ max_port_num ];
			void *_port_rec_fis_base[ max_port_num ];

		public:
			SataLs2k() : SataDriver() {};
			void init( const char * lock_name, void *clb, void *fb );
			int ahci_host_init();
			void set_port_cl( uint port, void *base ) { _port_cmd_lst_base[ port ] = base; }
			void set_port_fb( uint port, void *base ) { _port_rec_fis_base[ port ] = base; }

			struct HbaCmdHeader* get_cmd_header( uint port, uint head_index );

			void clear_port_intr( uint port, HbaRegPortIs intr );
			bool request_port_intr( uint port, HbaRegPortIs intr );
			inline uint32 get_port_is( uint port ) { return _hba_port_reg[ port ]->is; }

			void start_send_cmd( uint port, uint cmd_slot );
			bool request_cmd_finish( uint port, uint cmd_slot );

			void simple_init();

			void sata_probe();

		public:
			void debug_print_cmd_lst_base();
			void debug_print_rec_fis_base();
			void debug_print_port_d2h_fis( uint i );

			HbaPortReg *debug_get_port_reg_base( uint port ) { return _hba_port_reg[ 0 ]; }

		private:
			void _print_ahci_info();
			void _start_port( uint i );
		};

		extern SataLs2k k_sata_driver;
	} // namespace sata

} // namespace ata

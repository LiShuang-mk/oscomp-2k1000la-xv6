//
// Created by Li shuang ( pseudonym ) on 2024-04-14 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "smp/lock.hh"

namespace ata
{
	namespace sata
	{
		struct HbaCmdTbl;
	} // namespace sata

} // namespace ata



namespace dev
{
	namespace ahci
	{
		class AhciController
		{
		private:
			smp::Lock _lock;
			struct ata::sata::HbaCmdTbl *_cmd_tbl = nullptr;
			void *_pr = nullptr;
			int _is_idtf = false;

		public:
			AhciController() = default;
			void init( const char *lock_name );

			void isu_cmd_identify( uint port );

			void isu_cmd_read_dma( uint64 lba );

			void simple_intr_handle();

		};

		extern AhciController k_ahci_ctl;
	} // namespace ahci

} // namespace dev

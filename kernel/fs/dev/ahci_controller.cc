//
// Created by Li shuang ( pseudonym ) on 2024-04-14 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#include "hal/sata/fis.hh"
#include "hal/sata/hba_cmd.hh"
#include "hal/ata/ata_cmd.hh"
#include "fs/dev/ahci_controller.hh"
#include "mm/physical_memory_manager.hh"
#include "klib/common.hh"

namespace dev
{
	namespace ahci
	{
		AhciController k_ahci_ctl;

		void AhciController::init( const char *lock_name )
		{
			_lock.init( lock_name );
		}


		void AhciController::isu_cmd_identify()
		{
			struct ata::sata::HbaCmdTbl *cmd_tbl = ( struct ata::sata::HbaCmdTbl * cmd_tbl ) mm::k_pmm.alloc_page();
			assert( ( uint64 ) cmd_tbl != 0x0UL );

			struct ata::sata::FisRegH2D *fis_h2d = ( struct ata::sata::FisRegH2D * ) cmd_tbl->cmd_fis;
			fis_h2d->fis_type = ata::sata::FisType::fis_reg_h2d;
			fis_h2d->command = ata::AtaCmd::cmd_indentify_device;

			// to do 
		}
	} // namespace ahci

} // namespace dev

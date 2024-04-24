//
// Created by Li shuang ( pseudonym ) on 2024-04-08 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#pragma once 

#include "hal/sata/fis.hh"

namespace ata
{
	namespace sata
	{
		constexpr uint hba_rev_siz = 0x100;

		enum HbaRevOrg : uint
		{
			hba_fis_dma_beg = 0x00,
			hba_fis_dma_end = 0x1C,
			hba_fis_pio_beg = 0x20,
			hba_fis_pio_end = 0x34,
			hba_fis_d2h_beg = 0x40,
			hba_fis_d2h_end = 0x54,
			hba_fis_sdb_beg = 0x58,
			hba_fis_sdb_end = 0x60,
			hba_fis_unk_beg = 0x60,
			hba_fis_unk_end = 0xA0,
		};

		struct HbaRevFis
		{
			byte dsfis[ hba_fis_dma_end - hba_fis_dma_beg ];
			byte rsv0[ hba_fis_pio_beg - hba_fis_dma_end ];
			byte psfis[ hba_fis_pio_end - hba_fis_pio_beg ];
			byte rsv1[ hba_fis_d2h_beg - hba_fis_pio_end ];
			byte rfis[ hba_fis_d2h_end - hba_fis_d2h_beg ];
			byte rsv2[ hba_fis_sdb_beg - hba_fis_d2h_end ];
			byte sdbfis[ hba_fis_sdb_end - hba_fis_sdb_beg ];
			byte ufis[ hba_fis_unk_end - hba_fis_unk_beg ];
			byte rsv3[ hba_rev_siz - hba_fis_unk_end ];
		}__attribute__( ( __packed__ ) );
	} // namespace sata

} // namespace ata

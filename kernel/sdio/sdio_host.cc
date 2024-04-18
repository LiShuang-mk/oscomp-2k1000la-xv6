#include "sdio/sdio_host.hh"
#include "klib/common.hh"
#include "hal/loongarch.hh"
#include "hal/qemu_ls2k.hh"
namespace sdio{

    sdio_host k_sdio_host;

    void sdio_host::init(uint64 base, const char *name){
        _base = base;
        name = name;
    }

    void sdio_host::sdio_init(){
        // //set_sdio_mode();  //set SDIO mode
        uint64 ger_cfg_reg = 0;
        ger_cfg_reg =  read_GENERAL_CFG_REG();
        ger_cfg_reg |= GENERAL_CFG_REG::SDIO_SEL;
        write_GENERAL_CFG_REG(ger_cfg_reg);
        ger_cfg_reg = read_GENERAL_CFG_REG();

        //while(read_reg32(_SDI_CON_) & SDI_CON::_soft_rst_);  //wait for reset finish

    }

    void sdio_host::request(){
        //TODO
    }

    void sdio_host::set_iocfg(){
        //TODO
    }

    int sdio_host::get_card_status(){
        //TODO
        return 0;
    }

    void sdio_host::enable_sdio_irq(){
        //TODO
    }

    int sdio_host::execute_tuning(){
        //TODO
        return 0;
    }

    void sdio_host::init_Card(){
        uint8 r1;
        uint16 retry;
        uint8 buf[4];
        uint16 i;

        //sdio_init();
        for(i=0; i<10; i++)
        // 写 0xFF
        retry = 20;
        do
        {
            r1 = SDSendCmd(sdio_cmd::GO_IDLE_STATE, 0, 0x95);
        }
        while((r1 != 0x01) && retry--);
        //SD_Type = 0;
        if(r1 == 0x01)
        {
            if(SDSendCmd(sdio_cmd::SEND_IF_COND, 0x1AA, 0x87) == 0x01)
            {
                for(i=0; i<4; i++)
                {
                    //buf[i] = SDSpiREadWriteByte(0xff);
                    buf[i] = 0xff;
                }
                if(buf[2] == 0x01 && buf[3] == 0xAA){
                    retry = 0xFFFE;
                    do
                    {
                        SDSendCmd(sdio_cmd::APP_CMD, 0, 0x01);
                        r1 = SDSendCmd(sdio_cmd::SD_SEND_OP_COND, 0x40000000, 0x01);
                    }while(r1 && retry--);

                    // if(retry && SDSendCmd(sdio_cmd::READ_OCR, 0, 0x01) == 0)
                    // {
                    // }
                }//fe00420
            }
            
        }
        //TODO
    }

    void sdio_host::write_reg(uint32 reg, uint8 cmd){
        volatile uint32 *reg_addr = (uint32 *)(_base + reg);
        *reg_addr = cmd;
    }

    uint32 sdio_host::read_reg32(uint64 reg){
        return *( volatile uint32 * ) ((_base + reg) | loongarch::qemuls2k::dmwin::win_0);
    }

    uint64 sdio_host::read_GENERAL_CFG_REG(){
        return *( volatile uint64 * ) (_GENERAL_CFG_REG_ | loongarch::qemuls2k::dmwin::win_0);
    }

    void sdio_host::write_GENERAL_CFG_REG(uint64 _data){
        *( volatile uint64 * ) (_GENERAL_CFG_REG_ | loongarch::qemuls2k::dmwin::win_0) = _data;
    }

    // uint32 send_cmd(sdio_cmd cmd, uint32 arg){
    //     write_reg(_SDI_CMD_ARG_, arg);
    //     write_reg(_SDI_CMD_CON, cmd);
    //     while(read_reg(_SDI_CMD_STA) & 0x1);
    //     return 0;
    // }

    // void sdio_host::set_sdio_mode(){
    //     *( volatile  uint64 * ) (0x1fe00420) = (*( volatile uint64 *) (0x1fe00420) |= GENERAL_CFG_REG::SDIO_SEL)
	// 	= data;
    // }
    uint8 sdio_host::SDSendCmd(uint8 cmd, uint32 arg, uint8 crc)
    {
        // uint8 r1;
        // uint16 retry = 0;
        // SD_DisSelect();
        // if(SD_Select()) return 0xff;
        // uint8 buf[6];
        // uint16 i;
        // buf[0] = 0x40 | cmd;
        // buf[1] = arg >> 24;
        // buf[2] = arg >> 16;
        // buf[3] = arg >> 8;
        // buf[4] = arg;
        // buf[5] = crc;
        // for(i=0; i<6; i++)
        // {
        //     SdioReadWriteByte(buf[i]);
        // }
        // while(retry < 10)
        // {
        //     r1 = SdioReadWriteByte(0xff);
        //     if(r1 != 0xff)
        //     {
        //         return r1;
        //     }
        //     retry++;
        // }
        return 0xff;
    }

    uint8 sdio_host::SdioReadWriteByte(uint8 data)
    {
        write_reg(_SDI_CMD_ARG_, data);
        while(read_reg32(_SDI_CMD_STA) & 0x1)
        {
            //TODO
        }
        return read_reg32(_SDI_RSP_0_);

    }
}
#include "../types.hh"
#include "sdio_reg.hh"
#include "sdio_cmd.hh"

namespace sdio{

    class mmcsd_req;

    enum OcrEnum{
        VDD_165_195 = (1 << 7),
        VDD_20_21 = (1 << 8),
        VDD_21_22 = (1 << 9),
        VDD_22_23 = (1 << 10),
        VDD_23_24 = (1 << 11),
        VDD_24_25 = (1 << 12),
        VDD_25_26 = (1 << 13),
        VDD_26_27 = (1 << 14),
        VDD_27_28 = (1 << 15),
        VDD_28_29 = (1 << 16),
        VDD_29_30 = (1 << 17),
        VDD_30_31 = (1 << 18),
        VDD_31_32 = (1 << 19),
        VDD_32_33 = (1 << 20),
        VDD_33_34 = (1 << 21),
        VDD_34_35 = (1 << 22),
        VDD_35_36 = (1 << 23),
    };

    class sdio_host{
        private:
            uint64 _base;  // 0x1fe2c000
            const char* name;
            //mmcsd_card *card;
            uint32 freq_min;
            uint32 freq_max;
            OcrEnum valid_ocr;

            uint32 max_dma_segs;    //max number of dma segs in one request
            uint32 max_seg_size;    //max size of one dma seg
            uint32 max_blk_size;
            uint32 max_blk_count;
            
            uint32 id;

            //uint32 sdio_irq_num;
            //pm::Pcb thread;

        public:
            sdio_host() = default;              
            sdio_host(uint64 base, const char *name) : _base(base), name(name) {};
            void init(uint64 base, const char *name);           //init host
            ~sdio_host() = default;
            void sdio_init();
            void request();
            void set_iocfg();
            int get_card_status();
            void enable_sdio_irq();
            int execute_tuning();
            void init_Card();
            void write_reg(uint32 reg, uint8 cmd);
            uint32 read_reg32(uint64 reg);
            uint64 read_GENERAL_CFG_REG();
            void write_GENERAL_CFG_REG(uint64 val);
            void set_sdio_mode();
            uint8 SDSendCmd(uint8 cmd, uint32 arg, uint8 crc);
            int SD_DisSelect();
            int SD_Select();
            uint8 SdioReadWriteByte(uint8 data);
    };
    
    extern sdio_host k_sdio_host;
}

 // stio controler base address is 0x1fe2c000
 // config general reg 0 address is 0x1fe00420 [20] sdio_sel / geio_sel
namespace sdio{

    enum sdio_cmd{
        //Class 0,2,4,5,8 are imposed to support in every SDcard


        // class 0

        GO_IDLE_STATE = 0,          //bc
            //Reserved_1
        ALL_SEND_CID = 2,           //bcr                  R2
        SET_RELATIVE_ADDR = 3,      //ac  [31:16] RCA      R1
        SET_DSR = 4,                //bc  [31:16] RCA      R1b
            //Reserved_5_I/O_Card
            //SWITCH = 6,               //ac  [31:0] See below R1b     R1
        SELECT_CARD = 7,            //ac  [31:16] RCA      R1
        SEND_IF_COND = 8,           //bcr [11:8]vcc,[7:0]proj R7                 R1
        SEND_CSD = 9,               //ac  [31:16] RCA      R2
        SEND_CID = 10,              //ac  [31:16] RCA      R2
        VOLTAGE_SWITCH = 11,        //ac                   R1
        STOP_TRANSMISSION = 12,     //ac                   R1b
        SEND_STATUS = 13,           //ac                   R1
            //Reserved_14
        GO_INACTIVE_STATE = 15,     //ac  [31:16] RCA      R1

        // class II (block oriented read commands)

        SET_BLOCKLEN = 16,          //ac  [31:0] block len R1
        READ_SINGLE_BLOCK = 17,     //adtc [31:0] data addr R1
        READ_MULTIPLE_BLOCK = 18,   //adtc [31:0] data addr R1
        SEND_TUNING_BLOCK = 19,     //adtc                  R1
        SPEED_CLASS_CONTROL = 20,   //adtc [31:28]speed     R1
            //Reserved_21
            //Reserved_22
        SET_BLOCK_COUNT = 23,       //ac  [31:0] block cnt R1

        // class IV (block oriented write commands)
        WRITE_BLOCK = 24,           //adtc [31:0] data addr R1
        WRITE_MULTIPLE_BLOCK = 25,  //adtc [31:0] data addr R1
            //Reserved_26
        PROGRAM_CSD = 27,           //adtc                  R1
        
        // class VI (block oriented write protection commands)
        SET_WRITE_PROT = 28,        //ac  [31:0] data addr R1b
        CLR_WRITE_PROT = 29,        //ac  [31:0] data addr R1b
        SEND_WRITE_PROT = 30,       //adtc [31:0] data addr R1
            //Reserved_31

        // class V (erase card)
        ERASE_WR_BLK_START = 32,    //ac  [31:0] data addr R1
        ERASE_WR_BLK_END = 33,      //ac  [31:0] data addr R1
        ERASE = 38,                 //ac                   R1b
            //Reserved_39
            //Reserved_41

        // class VII (lock card)
            //Reserved_16
            //Reserved_40               //adtc
        LOCK_UNLOCK = 42,           //adtc [31:0] data addr R1
            //Reserved_43
            //Reserved_44
            //Reserved_45
            //Reserved_46
            //Reserved_47
            //Reserved_48
            //Reserved_49
            //Reserved_51

        // class VIII (special program commands)
        APP_CMD = 55,               //ac  [31:16] RCA      R1
        GEN_CMD = 56,               //adtc [31:0] arg       R1
            //Reserved_58
            //Reserved_59
            //Reserved_60
            //Reserved_61
            //Reserved_62
            //Reserved_63

        //SD commands

        //class 0
        SET_BUS_WIDTH = 6,          //ac  [1:0] bus width   R1
        SD_STATUS = 13,             //ac                   R1
        SEND_NUM_WR_BLOCKS = 22,    //adtc [31:0] data addr R1
        SET_WR_BLK_ERASE_COUNT = 23,//ac  [31:0] data addr R1
        SD_SEND_OP_COND = 41,       //ac  [31:0] OCR       R3
        SET_CLR_CARD_DETECT = 42,   //adtc [31:0] data addr R1
        SEND_SCR = 51,              //adtc                  R1
    };

    enum sd_response {
        MSD_RESPONSE_NO_ERROR = 0x00,          // No error
        MSD_RESPONSE_IN_IDLE_STATE = 0x01,     // In idle state
        MSD_RESPONSE_ERASE_RESET = 0x02,       // Erase reset
        MSD_RESPONSE_ILLEGAL_COMMAND = 0x04,   // Illegal command
        MSD_RESPONSE_COM_CRC_ERROR = 0x08,     // Com CRC error
        MSD_RESPONSE_ERASE_SEQUENCE_ERROR = 0x10, // Erase sequence error
        MSD_RESPONSE_ADDRESS_ERROR = 0x20,     // Address error
        MSD_RESPONSE_PARAMETER_ERROR = 0x40,   // Parameter error
        MSD_RESPONSE_RESPONSE_FAILURE = 0xFF,    // Response error
    };

    enum sd_type {
        SD_TYPE_UNKNOWN = 0,
        SD_TYPE_MMC = 1,
        SD_TYPE_V1 = 2,
        SD_TYPE_V2 = 3,
        SD_TYPE_V2HC = 3,
        
    };
}
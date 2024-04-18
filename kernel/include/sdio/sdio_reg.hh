

namespace sdio
{
  enum sdioregs{
    _GENERAL_CFG_REG_ = 0x1fe00420, //General configuration register
    _SDI_CON_ = 0X00,           //SDIO control register
    _SDI_PRE_ = 0X04,           //SDIO prescaler register
    _SDI_CMD_ARG_ = 0X08,       //SDIO command argument register
    _SDI_CMD_CON = 0X0C,        //SDIO command control register
    _SDI_CMD_STA = 0X10,        //SDIO command status register
    _SDI_RSP_0_ = 0X14,         //SDIO response register 0 , [31:0] -> short cmd , [127:96] long cmd
    _SDI_RSP_1_ = 0X18,         //SDIO response register 1 , [95:64] ->long cmd
    _SDI_RSP_2_ = 0X1C,         //SDIO response register 2 , [63:32] ->long cmd
    _SDI_RSP_3_ = 0X20,         //SDIO response register 3 , [31:0] ->long cmd
    _SDI_DATA_TMR_ = 0X24,      //SDIO data timer register
    _SDI_B_SIZE_ = 0X28,        //SDIO block size register
    _SDI_DAT_CON_ = 0X2C,       //SDIO data control register
    _SDI_DAT_CNT_ = 0X30,       //SDIO data counter register
    _SDI_DAT_STA_ = 0X34,       //SDIO data status register
    _SDI_FIFO_STA_ = 0X38,      //SDIO FIFO status register
    _SDI_INT_MASK_ = 0X3C,      //SDIO interrupt register
    _SDI_DAT_ = 0X40,           //SDIO command data register
    _SDI_INT_EN_ = 0X64,        //SDIO interrupt enable register
  };

  enum GENERAL_CFG_REG{
    SDIO_SEL = 1 << 20,
    GEIO_SEL = 0 << 20,
  };

  enum SDI_CON{
    _soft_rst_ = 1 << 8,
    _enclk_ = 1 << 0,
  };
} 

################################################################
# >>>>    file name : HSAI configure definition
# >>>> created time : 2024-06-29
# >>>>       author : lishuang-mk
# >>>>       e-mail : lishuang.mk@whu.edu.cn
# >>>>  description : To config what should be compiled into 
#                     HSAI module. It is apllied to clip or 
#                     trim the HSAI module.
# >>>>  PLEASE NOTE : This file is for learning purposes only
################################################################

# **************************************************************
# >>>> change these viriable to control what to be compiled
# >>>>

HSAI_DEF_UART = \
	virtual_uart.cc \
	uart_ns16550.cc

HSAI_DEF_ATA = \
	# ahci_driver.cc \
	# ahci_port_driver.cc

# >>>>
# **************************************************************
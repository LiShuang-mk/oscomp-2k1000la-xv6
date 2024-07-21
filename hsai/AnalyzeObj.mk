################################################################
# >>>>    file name : Analyze Object files make
# >>>> created time : 2024-07-01
# >>>>       author : lishuang-mk
# >>>>       e-mail : lishuang.mk@whu.edu.cn
# >>>>  description : Analyze what to compile in HSAI module.
#                     It includes some make functions.
# >>>>  PLEASE NOTE : This file is for learning purposes only
################################################################

# **************************************************************
# *   target module : Uart

hsai_get_src_uart = $(foreach obj,$(HASI_DEF_UART),uart/$(obj))

# **************************************************************




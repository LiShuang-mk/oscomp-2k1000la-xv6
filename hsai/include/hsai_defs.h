//
// Created by Li shuang ( pseudonym ) on 2024-07-13 
// --------------------------------------------------------------
// | Note: This code file just for study, not for commercial use 
// | Contact Author: lishuang.mk@whu.edu.cn 
// --------------------------------------------------------------
//

#ifndef _HSAI_DEFS_H_
#define _HSAI_DEFS_H_

#define __hsai_kernel
#define __hsai_hal

#ifndef NUMCPU
#define NUMCPU 2
#endif

#define DEV_TBL_LEN 64			// 设备管理表最大长度
#define DEV_FIRST_NOT_RSV	3	// 第一个未保留的设备
#define DEV_STDIN_NUM		0	// 标准输入流设备号
#define DEV_STDOUT_NUM		1	// 标准输出流设备号
#define DEV_STDERR_NUM		2	// 标准错误流设备号

#define DEFAULT_DEBUG_CONSOLE_NAME "console"

#endif // _HSAI_DEFS_H_
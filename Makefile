# configure 
CONF_CPU_NUM = 1
export CONF_ARCH ?= loongarch
export CONF_PLATFORM ?= qemu_2k1000
# CONF_LINUX_BUILD = 1

# make variable define 

HOST_OS = $(shell uname)
HAL_LIB_NAME = hal_${CONF_ARCH}_${CONF_PLATFORM}.a

# 带有export的变量会在递归调用子目录的Makefile时传递下去

export TOOLPREFIX = loongarch64-linux-gnu-
export DEFAULT_CXX_INCLUDE_FLAG = \
	-include $(WORKPATH)/kernel/include/xn6_config.h \
	-include $(WORKPATH)/kernel/include/klib/virtual_function.hh \
	-include $(WORKPATH)/kernel/include/klib/global_operator.hh \
	-include $(WORKPATH)/kernel/include/types.hh \
	-include $(WORKPATH)/hsai/include/hsai_defs.h

export CC  = ${TOOLPREFIX}gcc
export CXX = ${TOOLPREFIX}g++
export CPP = ${TOOLPREFIX}cpp
export AS  = ${TOOLPREFIX}g++
export LD  = ${TOOLPREFIX}g++
export OBJCOPY = ${TOOLPREFIX}objcopy
export OBJDUMP = ${TOOLPREFIX}objdump
export AR  = ${TOOLPREFIX}ar

export ASFLAGS = -march=loongarch64 -mabi=lp64d -g -nostdlib
export ASFLAGS += -I ./include
export ASFLAGS += -MD 
export CFLAGS = -g -Wall -Werror -O0 -fno-omit-frame-pointer
export CFLAGS += -I ./include
export CFLAGS += -MD 
export CFLAGS += -DNUMCPU=$(CONF_CPU_NUM)
export CFLAGS += -DARCH=$(CONF_ARCH)
export CFLAGS += -DPLATFORM=$(CONF_PLATFORM)
export CFLAGS += -DOS_DEBUG										# open debug output
ifeq ($(HOST_OS),Linux)
export CFLAGS += -DLINUX_BUILD=1
endif
export CFLAGS += -march=loongarch64 -mabi=lp64d
export CFLAGS += -ffreestanding -fno-common -nostdlib -fno-stack-protector 
export CFLAGS += -fno-pie -no-pie 
# export CFLAGS += -static-libstdc++ -lstdc++
export CXXFLAGS = $(CFLAGS)
export CXXFLAGS += -std=c++23
export CXXFLAGS += $(DEFAULT_CXX_INCLUDE_FLAG)
export LDFLAGS = -z max-page-size=4096

export WORKPATH = $(shell pwd)
export BUILDPATH = $(WORKPATH)/build


STATIC_MODULE = \
	$(BUILDPATH)/kernel.a \
	$(BUILDPATH)/user/user.a \
	$(BUILDPATH)/thirdparty/EASTL/libeastl.a \
	$(BUILDPATH)/hsai.a \
	$(BUILDPATH)/$(HAL_LIB_NAME)

LD_SCRIPT = hal/$(CONF_ARCH)/$(CONF_PLATFORM)/ld.script

COMPILE_START_TIME := $(shell cat /proc/uptime | awk -F '.' '{print $$1}')


# .PHONY 是一个伪规则，其后面依赖的规则目标会成为一个伪目标，使得规则执行时不会实际生成这个目标文件
.PHONY: all clean test initdir probe_host compile_all load_kernel EASTL EASTL_test config_platform print_compile_time

# rules define 

all: probe_host compile_all load_kernel
	$(OBJCOPY) -O binary ./build/kernel.elf ./kernel.bin
	@current_time=`cat /proc/uptime | awk -F '.' '{print $$1}'`; \
	echo "######## 生成结束时间戳: $${current_time} ########"; \
	echo "生成结束时间戳: $${current_time}"; \
	time_interval=`expr $${current_time} - $(COMPILE_START_TIME)`; \
	runtime=`date -u -d @$${time_interval} "+%Mm %Ss"`; \
	echo "######## 生成用时 : $${runtime} ########"
	@echo "__________________________"
	@echo "-------- 生成成功 --------"

probe_host:
	@echo "********************************"
	@echo "当前主机操作系统：$(HOST_OS)"
	@echo "编译目标平台：$(CONF_ARCH)-$(CONF_PLATFORM)"
	@echo "当前时间戳: $(COMPILE_START_TIME)"
	@echo "********************************"

compile_all:
	$(MAKE) all -C thirdparty/EASTL
	$(MAKE) all -C user
	$(MAKE) all -C hsai
	$(MAKE) all -C hal/$(CONF_ARCH)
	$(MAKE) all -C kernel

load_kernel: $(BUILDPATH)/kernel.elf

$(BUILDPATH)/kernel.elf: $(STATIC_MODULE) $(LD_SCRIPT)
	$(LD) $(LDFLAGS) -T $(LD_SCRIPT) -o $@ -Wl,--whole-archive $(STATIC_MODULE) -Wl,--no-whole-archive

test:
	@echo $(COMPILE_START_TIME); sleep 3; \
	current_time=`cat /proc/uptime | awk -F '.' '{print $$1}'`; echo $${current_time}; \
	echo `expr $${current_time} - $(COMPILE_START_TIME)`
# $(MAKE) test -C hsai
#	$(MAKE) test -C kernel

print_inc:
	$(CPP) $(DEFAULT_CXX_INCLUDE_FLAG) -v /dev/null -o /dev/null

print_compile_time:
	@current_time=`cat /proc/uptime | awk -F '.' '{print $$1}'`; \
	time_interval=`expr $${current_time} - $(COMPILE_START_TIME)`; \
	runtime=`date -u -d @$${time_interval} "+%Mm %Ss"`; \
	echo "######## 生成用时 : $${runtime} ########"

clean:
	$(MAKE) clean -C kernel
	$(MAKE) clean -C user
	$(MAKE) clean -C thirdparty/EASTL
	$(MAKE) clean -C hsai
	$(MAKE) clean -C hal/$(CONF_ARCH)

EASTL_test:
	$(MAKE) test -C thirdparty/EASTL

# config_platform:
# 	@cd hal/$(CONF_ARCH)/$(CONF_PLATFORM); \
# 		cp config.mk $(WORKPATH)/hsai/Makedefs.mk
# 	@echo "******** 配置成功 ********"
# 	@echo "- 架构 : ${CONF_ARCH}"
# 	@echo "- 平台 : ${CONF_PLATFORM}"
# 	@echo "**************************"

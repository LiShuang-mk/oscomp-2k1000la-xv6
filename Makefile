# configure 
CONF_CPU_NUM = 1
CONF_ARCH = loongarch 
CONF_PLATFORM = ls2k
# CONF_LINUX_BUILD = 1

# make variable define 

HOST_OS = $(shell uname)

# 带有export的变量会在递归调用子目录的Makefile时传递下去

export TOOLPREFIX = loongarch64-linux-gnu-

export CC  = ${TOOLPREFIX}gcc
export CXX = ${TOOLPREFIX}g++
export AS  = ${TOOLPREFIX}as
export LD  = ${TOOLPREFIX}g++
export OBJCOPY = ${TOOLPREFIX}objcopy
export OBJDUMP = ${TOOLPREFIX}objdump
export AR = ${TOOLPREFIX}ar

export ASFLAGS = -march=loongarch64 -mabi=lp64d
export ASFLAGS += -I ./include
export ASFLAGS += -MD 
export CFLAGS = -Wall -Werror -O0 -fno-omit-frame-pointer -ggdb
export CFLAGS += -MD 
export CFLAGS += -DNUMCPU=$(CONF_CPU_NUM)
export CFLAGS += -DARCH=$(CONF_ARCH)
export CFLAGS += -DPLATFORM=$(CONF_PLATFORM)
ifeq ($(HOST_OS),Linux)
export CFLAGS += -DLINUX_BUILD=1
endif
export CFLAGS += -march=loongarch64 -mabi=lp64d
export CFLAGS += -ffreestanding -fno-common -nostdlib
export CFLAGS += -I ./include -fno-stack-protector 
export CFLAGS += -fno-pie -no-pie 
# export CFLAGS += -static-libstdc++ -lstdc++
export CXXFLAGS = $(CFLAGS)
export CXXFLAGS += -std=c++23
export LDFLAGS = -z max-page-size=4096  

export WORKPATH = $(shell pwd)
export BUILDPATH = $(WORKPATH)/build

# .PHONY 是一个伪规则，其后面依赖的规则目标会成为一个伪目标，使得规则执行时不会实际生成这个目标文件
.PHONY: all clean test initdir

# rules define 

all: initdir
	@echo "当前主机操作系统：$(HOST_OS)"
	$(MAKE) -C kernel
	@echo "_________________________"
	@echo "-------- 生成成功 --------"

initdir:
	$(MAKE) initdir -C kernel;$(MAKE) initdir -C thirdparty/EASTL

test:
	$(MAKE) test -C kernel

clean:
	$(MAKE) clean -C kernel; $(MAKE) clean -C thirdparty/EASTL

EASTL:
	$(MAKE) -C thirdparty/EASTL
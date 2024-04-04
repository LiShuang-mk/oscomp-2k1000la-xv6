# K = kernel
# U = user

# configure 
CONF_CPU_NUM = 1
# CONF_LINUX_BUILD = 1

# make variable define 

HOST_OS = $(shell uname)

export TOOLPREFIX = loongarch64-linux-gnu-

export CC  = ${TOOLPREFIX}gcc
export CXX = ${TOOLPREFIX}g++
export AS  = ${TOOLPREFIX}as
export LD  = ${TOOLPREFIX}ld 
export OBJCOPY = ${TOOLPREFIX}objcopy
export OBJDUMP = ${TOOLPREFIX}objdump

export ASFLAGS = -march=loongarch64 -mabi=lp64s
export ASFLAGS += -I ./include
export ASFLAGS += -MD 
export CFLAGS = -Wall -Werror -O0 -fno-omit-frame-pointer -ggdb
export CFLAGS += -MD 
export CFLAGS += -DNUMCPU=$(CONF_CPU_NUM)
ifeq ($(HOST_OS),Linux)
export CFLAGS += -DLINUX_BUILD=1
endif
export CFLAGS += -march=loongarch64 -mabi=lp64s
export CFLAGS += -ffreestanding -fno-common -nostdlib
export CFLAGS += -I ./include -fno-stack-protector
export CFLAGS += -fno-pie -no-pie
export CXXFLAGS = $(CFLAGS)
# export CXXFLAGS += -std=gnu++23
export LDFLAGS = -z max-page-size=4096

export WORKPATH = $(shell pwd)
export BUILDPATH = $(WORKPATH)/build

.PHONY: all clean test 

# rules define 

all:
	@echo "当前主机操作系统：$(HOST_OS)"
	$(MAKE) -C kernel
	@echo "_________________________"
	@echo "-------- 生成成功 --------"

initdir:
	cd kernel; make initdir

test:
	$(MAKE) test -C kernel

clean:
	$(MAKE) clean -C kernel
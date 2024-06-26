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
export AR  = ${TOOLPREFIX}ar

export ASFLAGS = -march=loongarch64 -mabi=lp64d
export ASFLAGS += -I ./include
export ASFLAGS += -MD 
export CFLAGS = -Wall -Werror -O0 -fno-omit-frame-pointer -ggdb
export CFLAGS += -MD 
export CFLAGS += -DNUMCPU=$(CONF_CPU_NUM)
export CFLAGS += -DARCH=$(CONF_ARCH)
export CFLAGS += -DPLATFORM=$(CONF_PLATFORM)
# export CFLAGS += -DOS_DEBUG
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
export CXXFLAGS += -include $(WORKPATH)/kernel/include/klib/virtual_function.hh
export CXXFLAGS += -include $(WORKPATH)/kernel/include/klib/global_operator.hh
export LDFLAGS = -z max-page-size=4096  

export WORKPATH = $(shell pwd)
export BUILDPATH = $(WORKPATH)/build


STATIC_MODULE = \
	$(BUILDPATH)/kernel.a \
	$(BUILDPATH)/user/user.a \
	$(BUILDPATH)/thirdparty/EASTL/libeastl.a


# .PHONY 是一个伪规则，其后面依赖的规则目标会成为一个伪目标，使得规则执行时不会实际生成这个目标文件
.PHONY: all clean test initdir probe_host compile_all load_kernel EASTL EASTL_test

# rules define 

all: initdir probe_host compile_all load_kernel
	$(OBJCOPY) -O binary ./build/kernel.elf ./kernel.bin
	@echo "__________________________"
	@echo "-------- 生成成功 --------"

initdir:
	$(MAKE) initdir -C kernel
	$(MAKE) initdir -C user
	$(MAKE) initdir -C thirdparty/EASTL

probe_host:
	@echo "********************************"
	@echo "当前主机操作系统：$(HOST_OS)"
	@echo "********************************"


compile_all:
	$(MAKE) EASTL
	$(MAKE) all -C user
	$(MAKE) all -C kernel

load_kernel: $(BUILDPATH)/kernel.elf

$(BUILDPATH)/kernel.elf: $(STATIC_MODULE) kernel/kernel.ld 
	$(LD) $(LDFLAGS) -T kernel/kernel.ld -o $@ $(STATIC_MODULE)

test:
	$(MAKE) test -C kernel

clean:
	$(MAKE) clean -C kernel
	$(MAKE) clean -C user
	$(MAKE) clean -C thirdparty/EASTL

EASTL:
	$(MAKE) -C thirdparty/EASTL

EASTL_test:
	$(MAKE) test -C thirdparty/EASTL
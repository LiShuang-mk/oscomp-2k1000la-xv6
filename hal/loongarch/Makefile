################################################################
# >>>>    file name : loongarch-make
# >>>> created time : 2024-06-29
# >>>>       author : lishuang-mk
# >>>>       e-mail : lishuang.mk@whu.edu.cn
# >>>>  description : compile the loongarch libcpu codes and
#                     call the target platform MAKE
# >>>>  PLEASE NOTE : This file is for learning purposes only
################################################################

# *******************************************************
# >>>> support platform
support_platform = qemu_2k1000 ls2k1000dp

# >>>> 
# *******************************************************

# *******************************************************
# >>>> make function
# >>>> 

# card src function
# -> $1: directory
# -> $2: file type 
card_src = $(foreach d,$(wildcard $1*),$(call card_src,$d/,$2)$(filter $2,$d))

# card all sub-directory, but ignore 'include'
# -> $1: directory to card all sub-direcotry, whose suffix should be '/', such as 'kernel/'
#        null means current directory
card_dir = $(foreach d, $(filter-out include/, $(wildcard $1*/)), $(call card_dir, $1$d) $d)

# >>>>
# ********************************************************

# ********************************************************
# >>>> compile flags
# >>>>

export CFLAGS += -I $(WORKPATH)/hsai/include/
export CXXFLAGS += -I $(WORKPATH)/hsai/include/

# >>>>
# ********************************************************

# check platform
plat_name = \
	$(if $(filter $(CONF_PLATFORM),$(support_platform)), \
	$(CONF_PLATFORM), \
	$(error platform '${CONF_PLATFORM}' is not supported)\
	)
plat_name := $(strip $(plat_name))

# architecture name
arch_name = $(CONF_ARCH)

# target name
target_name = hal_$(arch_name)_$(plat_name)

# card source
c_src = $(wildcard *.c)
cc_src = $(wildcard *.cc)
s_src = $(wildcard *.S)

srcs = $(c_src) $(cc_src) $(s_src)

# card obj file name
c_obj = $(patsubst %.c,$(BUILDPATH)/hal/$(arch_name)/%.o,$(c_src))
cc_obj = $(patsubst %.cc,$(BUILDPATH)/hal/$(arch_name)/%.o,$(cc_src))
s_obj = $(patsubst %.S,$(BUILDPATH)/hal/$(arch_name)/%.o,$(s_src))

objs = $(c_obj) $(cc_obj) $(s_obj)

# the source files' dependencies, created by gcc
depends = \
	$(patsubst %.cc, $(BUILDPATH)/hal/$(arch_name)/%.d, $(cc_src)) \
	$(patsubst %.c, $(BUILDPATH)/hal/$(arch_name)/%.d, $(c_src)) \
	$(patsubst %.S, $(BUILDPATH)/hal/$(arch_name)/%.d, $(s_src))


.PHONY: all clean test initdir add_libcpu compile_platform

all: initdir compile_platform add_libcpu

initdir:
	mkdir -p $(BUILDPATH)/hal/$(arch_name)

compile_platform:
	$(MAKE) all -C $(plat_name)

add_libcpu: $(BUILDPATH)/$(target_name).a

$(BUILDPATH)/$(target_name).a: $(objs)
	$(AR) rc $(BUILDPATH)/$(target_name).a $(objs)

$(cc_obj): $(BUILDPATH)/hal/$(arch_name)/%.o: %.cc 
	$(CXX) -c $(CXXFLAGS) -MF $(BUILDPATH)/hal/$(arch_name)/$*.d -o $@ $<

$(c_obj): $(BUILDPATH)/hal/$(arch_name)/%.o: %.c 
	$(CC) -c $(CFLAGS) -MF $(BUILDPATH)/hal/$(arch_name)/$*.d -o $@ $<

$(s_obj): $(BUILDPATH)/hal/$(arch_name)/%.o: %.S 
	$(AS) -c $(ASFLAGS) -MF $(BUILDPATH)/hal/$(arch_name)/$*.d -o $@ $<

-include $(depends)

test:
	@echo $(plat_name)
	@echo $(target_name)
#	@echo $(call test_wild, $(test_dir), %.$(test_cc_ext))
#	@echo $(call test_wild, $(test_dir), %.$(test_c_ext))
#	@echo $(call test_wild, $(test_dir), %.$(test_S_ext))
#	@echo "--------> depd  : $(depends)"
	@echo "--------> srcs  : $(c_src)"
	@echo "--------> srcs  : $(cc_src)"
# @echo "--------> srcs  : $(s_src)"
# @echo "--------> srcs  : $(srcs)"
#	@echo "--------> objs  : $(objs)"
#	@echo "--------> build : $(BUILDPATH)"

clean: 
	cd "$(BUILDPATH)/hal/$(arch_name)"; rm -f *.o *.d
	$(MAKE) clean -C $(plat_name)

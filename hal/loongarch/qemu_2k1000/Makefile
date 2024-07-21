################################################################
# >>>>    file name : qemu-2k1000-make
# >>>> created time : 2024-06-29
# >>>>       author : lishuang-mk
# >>>>       e-mail : lishuang.mk@whu.edu.cn
# >>>>  description : compile the qemu-2k1000 platform codes
#                     and package all object file into a library
# >>>>  PLEASE NOTE : This file is for learning purposes only
################################################################


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

ASFLAGS += -I ../include/
CFLAGS += -I ../include/
CXXFLAGS += -I ../include/

# >>>>
# ********************************************************

# architecture name
arch_name = $(CONF_ARCH)

# platform name 
plat_name = $(CONF_PLATFORM)

# library name
lib_name = hal_$(arch_name)_$(plat_name)

# card sub-module
sub_mod := $(call card_dir,) # $1 = null

# card source
c_src = $(foreach d, $(sub_mod), $(wildcard $(d)*.c)) $(wildcard *.c)
cc_src = $(foreach d, $(sub_mod), $(wildcard $(d)*.cc)) $(wildcard *.cc)
s_src = $(foreach d, $(sub_mod), $(wildcard $(d)*.S)) $(wildcard *.S)

srcs = $(c_src) $(cc_src) $(s_src)

# card obj file name
c_obj = $(patsubst %.c,$(BUILDPATH)/hal/$(arch_name)/$(plat_name)/%.o,$(c_src))
cc_obj = $(patsubst %.cc,$(BUILDPATH)/hal/$(arch_name)/$(plat_name)/%.o,$(cc_src))
s_obj = $(patsubst %.S,$(BUILDPATH)/hal/$(arch_name)/$(plat_name)/%.o,$(s_src))
objs = $(c_obj) $(cc_obj) $(s_obj)

# the source files' dependencies, created by gcc
depends = \
	$(patsubst %.cc, $(BUILDPATH)/hal/$(arch_name)/$(plat_name)/%.d, $(cc_src)) \
	$(patsubst %.c, $(BUILDPATH)/hal/$(arch_name)/$(plat_name)/%.d, $(c_src)) \
	$(patsubst %.S, $(BUILDPATH)/hal/$(arch_name)/$(plat_name)/%.d, $(s_src))


.PHONY: all clean test initdir

all: initdir $(BUILDPATH)/$(lib_name).a

initdir:
	mkdir -p $(BUILDPATH)/hal/$(arch_name)/$(plat_name)
	cd $(BUILDPATH)/hal/$(arch_name)/$(plat_name); for dir in $(sub_mod); do mkdir -p "$${dir}"; done

$(BUILDPATH)/$(lib_name).a: $(objs)
	$(AR) rcs $@ $^

$(cc_obj): $(BUILDPATH)/hal/$(arch_name)/$(plat_name)/%.o: %.cc 
	$(CXX) -c $(CXXFLAGS) -MF $(BUILDPATH)/hal/$(arch_name)/$(plat_name)/$*.d -o $@ $<

$(c_obj): $(BUILDPATH)/hal/$(arch_name)/$(plat_name)/%.o: %.c 
	$(CC) -c $(CFLAGS) -MF $(BUILDPATH)/hal/$(arch_name)/$(plat_name)/$*.d -o $@ $<

$(s_obj): $(BUILDPATH)/hal/$(arch_name)/$(plat_name)/%.o: %.S 
	$(CC) -c $(ASFLAGS) -MF $(BUILDPATH)/hal/$(arch_name)/$(plat_name)/$*.d -o $@ $<

-include $(depends)

test:
	@echo $(MAKEFILE_LIST)
	@echo $(platform_name)
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
	cd "$(BUILDPATH)/hal/$(arch_name)/$(plat_name)"; rm -f *.o *.d
	for mod in $(sub_mod) ; do \
		cd "$(BUILDPATH)/hal/$(arch_name)/$(plat_name)/$$mod"; rm -f *.o *.d; \
	done
	cd $(BUILDPATH); rm -f $(lib_name).a

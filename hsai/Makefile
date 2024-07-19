################################################################
# >>>>    file name : Hardware Service Abstract Interface Makefile
# >>>> created time : 2024-06-29
# >>>>       author : lishuang-mk
# >>>>       e-mail : lishuang.mk@whu.edu.cn
# >>>>  description : compile HSAI module into a library
# >>>>  PLEASE NOTE : This file is for learning purposes only
################################################################

# **************************************************************
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
# **************************************************************

# **************************************************************
# >>>> compile flags
# >>>>

CFLAGS += -I $(WORKPATH)/kernel/include
CXXFLAGS += -I $(WORKPATH)/kernel/include

# >>>>
# **************************************************************

-include $(WORKPATH)/hal/$(CONF_ARCH)/$(CONF_PLATFORM)/hsai_defs.mk

# **************************************************************
# >>>> get mudule's source code

hsai_src =
hsai_src += $(wildcard *.c)
hsai_src += $(wildcard *.cc)
hsai_src += $(wildcard *.S)

# ------------------------------------------------
# | target module : ata

hsai_src += $(foreach obj,$(HSAI_DEF_ATA),ata/$(obj))

# ------------------------------------------------
# | target module : intr

hsai_src += $(call card_src,intr,%.cc)

# ------------------------------------------------
# | target module : mem

hsai_src += $(call card_src,mem,%.cc)

# ------------------------------------------------
# | target module : smp

hsai_src += $(call card_src,smp,%.cc)

# ------------------------------------------------
# | target module : uart

hsai_src += $(foreach obj,$(HSAI_DEF_UART),uart/$(obj))

# ------------------------------------------------

# **************************************************************

# this module name
mod_name = hsai

# submod
submod = $(call card_dir,) # $1 = null

# card source
c_src = $(filter %.c,$(hsai_src))
cc_src = $(filter %.cc,$(hsai_src))
s_src = $(filter %.S,$(hsai_src))

# card obj file name
c_obj = $(patsubst %.c,$(BUILDPATH)/$(mod_name)/%.o,$(c_src))
cc_obj = $(patsubst %.cc,$(BUILDPATH)/$(mod_name)/%.o,$(cc_src))
s_obj = $(patsubst %.S,$(BUILDPATH)/$(mod_name)/%.o,$(s_src))
objs = $(c_obj) $(cc_obj) $(s_obj)

# the source files' dependencies, created by gcc
depends = \
	$(patsubst %.cc, $(BUILDPATH)/$(mod_name)/%.d, $(cc_src)) \
	$(patsubst %.c, $(BUILDPATH)/$(mod_name)/%.d, $(c_src)) \
	$(patsubst %.S, $(BUILDPATH)/$(mod_name)/%.d, $(s_src))


.PHONY: all clean test initdir

all: initdir $(BUILDPATH)/$(mod_name).a
# all: $(BUILDPATH)/kernel.elf

initdir:
	mkdir -p $(BUILDPATH)/$(mod_name)
	cd $(BUILDPATH)/$(mod_name); for dir in $(submod); do mkdir -p "$${dir}"; done

$(BUILDPATH)/$(mod_name).a: $(objs)
	$(AR) rcs $@ $^

$(cc_obj): $(BUILDPATH)/$(mod_name)/%.o: %.cc 
	$(CXX) -c $(CXXFLAGS) -MF $(BUILDPATH)/$(mod_name)/$*.d -o $@ $<

$(c_obj): $(BUILDPATH)/$(mod_name)/%.o: %.c 
	$(CC) -c $(CFLAGS) -MF $(BUILDPATH)/$(mod_name)/$*.d -o $@ $<

$(s_obj): $(BUILDPATH)/$(mod_name)/%.o: %.S 
	$(CC) -c $(ASFLAGS) -MF $(BUILDPATH)/$(mod_name)/$*.d -o $@ $<

-include $(depends)

test:
	@echo $(hsai_src)
	@echo $(submod)
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
	cd $(BUILDPATH)/$(mod_name)/; rm -f *.o *.d
	for mod in $(submod) ; do \
		cd "$(BUILDPATH)/$(mod_name)/$${mod}" ; \
		rm *.o *.d; \
	done
	cd $(BUILDPATH); rm $(mod_name).a

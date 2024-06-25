# CXXFLAGS += -I ../thirdparty/EASTL/include
# CXXFLAGS += -I ../kernel/include/klib
# CXXFLAGS += -Wno-deprecated-declarations
# CXXFLAGS += -I ../thirdparty/EASTL/test/packages/EABase/include/Common


# this module name
mod_name = hal_loongarch


# card src function
# -> $1: directory
# -> $2: file type 
card_src = $(foreach d,$(wildcard $1*),$(call card_src,$d/,$2)$(filter $2,$d))

# card all sub-directory, but ignore 'include'
# -> $1: directory to card all sub-direcotry, whose suffix should be '/', such as 'kernel/'
#        null means current directory
card_dir = $(foreach d, $(filter-out include/, $(wildcard $1*/)), $(call card_dir, $1$d) $d)

submod = $(call card_dir, ) # $1 = null

# card source
c_src = $(foreach d, $(submod), $(wildcard $(d)*.c)) $(wildcard *.c)
cc_src = $(foreach d, $(submod), $(wildcard $(d)*.cc)) $(wildcard *.cc)
s_src = $(foreach d, $(submod), $(wildcard $(d)*.S)) $(wildcard *.S)

srcs = $(c_src) $(cc_src) $(s_src)


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

all: $(BUILDPATH)/$(mod_name).a
# all: $(BUILDPATH)/kernel.elf

initdir:
	mkdir -p $(BUILDPATH)/$(mod_name)
	cd $(BUILDPATH); for dir in $(submod); do mkdir -p "$(BUILDPATH)/$(mod_name)/$$dir"; done

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
	for mod in $(submod) ; do \
		cd "$(BUILDPATH)/kernel/$$mod" ; \
		rm -f *.o *.d; \
		cd "$(WORKPATH)/kernel/$$mod" ; \
		rm -f *.o *.d; \
	done
	cd $(BUILDPATH); rm -f kernel.elf kernel/*.o kernel/*.d kernel.a kernel.dis

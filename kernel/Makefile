CXXFLAGS += -include ./include/klib/virtual_function.hh
CXXFLAGS += -include ./include/klib/global_operator.hh

#CXXFLAGS += -include ../thirdparty/EASTL/include/EASTL/
#CXXFLAGS += -L $(BUILDPATH)/thirdparty/EASTL/ -llibeastl.a

# card src function
# -> $1: directory
# -> $2: file type 
card_src = $(foreach d,$(wildcard $1*),$(call card_src,$d/,$2)$(filter $2,$d))

# card all sub-directory, but ignore 'include'
# -> $1: directory to card all sub-direcotry, whose suffix should be '/', such as 'kernel/'
#        null means current directory
card_dir = $(foreach d, $(filter-out include/, $(wildcard $1*/)), $(call card_dir, $1$d) $d)

submod = $(call card_dir, ) # $1 = null
# submod = $(call card_idr, kernel/) # means $1 = kernel/

# card source
c_src = $(foreach d, $(submod), $(wildcard $(d)*.c)) $(wildcard *.c)
cc_src = $(foreach d, $(submod), $(wildcard $(d)*.cc)) $(wildcard *.cc)
s_src = $(foreach d, $(submod), $(wildcard $(d)*.S)) $(wildcard *.S)

srcs = $(c_src) $(cc_src) $(s_src)


# card obj file name
c_obj = $(patsubst %.c,$(BUILDPATH)/kernel/%.o,$(c_src))
cc_obj = $(patsubst %.cc,$(BUILDPATH)/kernel/%.o,$(cc_src))
s_obj = $(patsubst %.S,$(BUILDPATH)/kernel/%.o,$(s_src))

objs = $(c_obj) $(cc_obj) $(s_obj)

# the source files' dependencies, created by gcc
depends = \
	$(patsubst %.cc, $(BUILDPATH)/kernel/%.d, $(cc_src)) \
	$(patsubst %.c, $(BUILDPATH)/kernel/%.d, $(c_src)) \
	$(patsubst %.S, $(BUILDPATH)/kernel/%.d, $(s_src))


.PHONY: all clean test initdir

all: $(BUILDPATH)/kernel.elf

initdir:
	mkdir -p $(BUILDPATH)
	cd $(BUILDPATH); for dir in $(submod); do mkdir -p "$(BUILDPATH)/kernel/$$dir"; done

# make rules example
# a.o : a.c
#(tab)$(CC) -c $(CFLAGS) -o $@ $<
#       It means that a.o is dependent on a.c
#       Whenever a.c is newer than a.o, the command in the line behind the rule will be executed
#       In the command, '$@' means a.0 while '$<' means a.c

$(BUILDPATH)/kernel.elf: $(objs) kernel.ld 
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(objs)

# mode match example
# $(objs) : %.o : %.c
#       $(objs) is a list of all of the object file name
#       %.o means '%' match the file name without suffix
#       It generates dependencies for each object file
#         which is correspondence to the source file with the same name

$(cc_obj): $(BUILDPATH)/kernel/%.o: %.cc 
	$(CXX) -c $(CXXFLAGS) -MF $(BUILDPATH)/kernel/$*.d -o $@ $<

$(c_obj): $(BUILDPATH)/kernel/%.o: %.c 
	$(CC) -c $(CFLAGS) -MF $(BUILDPATH)/kernel/$*.d -o $@ $<

$(s_obj): $(BUILDPATH)/kernel/%.o: %.S 
	$(CC) -c $(ASFLAGS) -MF $(BUILDPATH)/kernel/$*.d -o $@ $<

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
	cd $(BUILDPATH); rm -f kernel.elf kernel/*.o kernel/*.d
	cd $(WORKPATH); rm -f kernel.elf kernel/*.o kernel/*.d

#-rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
#	*.o *.d *.asm *.sym *.out
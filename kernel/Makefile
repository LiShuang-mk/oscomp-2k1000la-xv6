
#submod = hal smp klib mm pm im tm 

# card src function
# -> $1: directory
# -> $2: file type 
card_src = $(foreach d,$(wildcard $1*),$(call card_src,$d/,$2)$(filter $2,$d))

card_dir = $(foreach d, $(filter-out include/, $(wildcard $1*/)), $(call card_dir, $1$d) $d)

submod = $(call card_dir,)

#srcs = $(wildcard *.cc *.c *.S) 
#srcs += $(foreach dir, $(submod), \
#		$(wildcard $(dir)/*.cc $(dir)/*.c $(dir)/*.S) \ 
#		)

c_src = $(foreach d, $(submod), $(wildcard $(d)*.c)) $(wildcard *.c)
cc_src = $(foreach d, $(submod), $(wildcard $(d)*.cc)) $(wildcard *.cc)
s_src = $(foreach d, $(submod), $(wildcard $(d)*.S)) $(wildcard *.S)

srcs = $(c_src) $(cc_src) $(s_src)

c_obj = $(patsubst %.c,$(BUILDPATH)/kernel/%.o,$(c_src))
cc_obj = $(patsubst %.cc,$(BUILDPATH)/kernel/%.o,$(cc_src))
s_obj = $(patsubst %.S,$(BUILDPATH)/kernel/%.o,$(s_src))

objs = $(c_obj) $(cc_obj) $(s_obj)
	
# objs += $(foreach dir, $(submod), \
# 			$(patsubst %.cc, $(BUILDPATH)/%.o, $(wildcard $(dir)/*.cc)) \
# 			$(patsubst %.c, $(BUILDPATH)/%.o, $(wildcard $(dir)/*.c)) \
# 			$(patsubst %.S, $(BUILDPATH)/%.o, $(wildcard $(dir)/*.S)) \
# 			)

depends = \
	$(patsubst %.cc, $(BUILDPATH)/kernel/%.d, $(cc_src)) \
	$(patsubst %.c, $(BUILDPATH)/kernel/%.d, $(c_src)) \
	$(patsubst %.S, $(BUILDPATH)/kernel/%.d, $(s_src))

es_buildpath = $(subst /,\/,$(BUILDPATH))

.PHONY: all clean test initdirm,/

all: $(BUILDPATH)/kernel.elf

initdir:
	mkdir -p $(BUILDPATH)
	cd $(BUILDPATH); for dir in $(submod); do mkdir -p "$(BUILDPATH)/kernel/$$dir"; done

$(BUILDPATH)/kernel.elf: $(objs) kernel.ld 
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $(objs)
#	for obj in $(objs); do mv $$obj "$(BUILDPATH)/kernel/$$obj"; done 
#	mv kernel.elf $(BUILDPATH)/kernel.elf

$(cc_obj): $(BUILDPATH)/kernel/%.o: %.cc 
	$(CXX) -c $(CXXFLAGS) -MF $(BUILDPATH)/kernel/$*.d -o $@ $<
#	sed 's/$*.o/$(es_buildpath)\/$*.o/' $(BUILDPATH)/$*.d > $(BUILDPATH)/$*.d

$(c_obj): $(BUILDPATH)/kernel/%.o: %.c 
	$(CC) -c $(CFLAGS) -MF $(BUILDPATH)/kernel/$*.d -o $@ $<
#	sed 's/$*.o/$(es_buildpath)\/$*.o/' $(BUILDPATH)/$*.d > $(BUILDPATH)/$*.d

$(s_obj): $(BUILDPATH)/kernel/%.o: %.S 
	$(CC) -c $(ASFLAGS) -MF $(BUILDPATH)/kernel/$*.d -o $@ $<
#	sed 's/$*.o/$(es_buildpath)\/$*.o/' $(BUILDPATH)/$*.d > $(BUILDPATH)/$*.d

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
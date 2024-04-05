
submod = hal smp klib mm pm im tm 


srcs = $(wildcard *.cc *.c *.S) 
srcs += $(foreach dir, $(submod), \
		$(wildcard $(dir)/*.cc $(dir)/*.c $(dir)/*.S) \
		)

c_src = $(filter %.c,$(srcs))
cc_src = $(filter %.cc,$(srcs))
s_src = $(filter %.S,$(srcs))

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
	@echo "--------> depd  : $(depends)"
	@echo "--------> srcs  : $(c_src)"
	@echo "--------> srcs  : $(cc_src)"
	@echo "--------> srcs  : $(s_src)"
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
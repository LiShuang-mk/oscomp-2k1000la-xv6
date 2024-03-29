
submod = hal smp klib mm pm


srcs = $(wildcard *.cc *.c *.S) 
srcs += $(foreach dir, $(submod), \
		$(wildcard $(dir)/*.cc $(dir)/*.c $(dir)/*.S) \
		)

objs = \
	$(patsubst %.cc, %.o, $(filter %.cc, $(srcs))) \
	$(patsubst %.c, %.o, $(filter %.c, $(srcs))) \
	$(patsubst %.S, %.o, $(filter %.S, $(srcs)))

# objs += $(foreach dir, $(submod), \
# 			$(patsubst %.cc, $(BUILDPATH)/%.o, $(wildcard $(dir)/*.cc)) \
# 			$(patsubst %.c, $(BUILDPATH)/%.o, $(wildcard $(dir)/*.c)) \
# 			$(patsubst %.S, $(BUILDPATH)/%.o, $(wildcard $(dir)/*.S)) \
# 			)

# depends in common path 
depends = \
	$(patsubst %.cc, %.d, $(filter %.cc, $(srcs))) \
	$(patsubst %.c, %.d, $(filter %.c, $(srcs))) \
	$(patsubst %.S, %.d, $(filter %.S, $(srcs)))

# depends += $(foreach dir, $(submod), \
# 			$(patsubst %.cc, $(BUILDPATH)/%.d, $(wildcard $(dir)/*.cc)) \
# 			$(patsubst %.c, $(BUILDPATH)/%.d, $(wildcard $(dir)/*.c)) \
# 			$(patsubst %.S, $(BUILDPATH)/%.d, $(wildcard $(dir)/*.S)) \
# 			)


.PHONY: all clean test initdir

all: kernel 

kernel: $(objs) kernel.ld 
	$(LD) $(LDFLAGS) -T kernel.ld -o kernel.elf $(objs)
	for obj in $(objs); do mv $$obj "$(BUILDPATH)/kernel/$$obj"; done 
	mv kernel.elf $(BUILDPATH)/kernel.elf
	-rm -f $(depends)

-include $(depends)

initdir:
	cd $(BUILDPATH); for dir in $(submod); do mkdir -p "$(BUILDPATH)/kernel/$$dir"; done 

test:
	@echo "--------> srcs  : $(srcs)"
	@echo "--------> objs  : $(objs)"
	@echo "--------> build : $(BUILDPATH)"

clean: 
	for mod in $(submod) ; \
		do cd "$(BUILDPATH)/kernel/$$mod" ; \
		rm -f *.o ; \
	done
	cd $(BUILDPATH); rm -f kernel.elf

#-rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
#	*.o *.d *.asm *.sym *.out
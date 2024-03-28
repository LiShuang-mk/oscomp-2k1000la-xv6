
submod = hal smp klib mm


srcs = $(wildcard *.cc *.c *.S)
objs = \
	$(patsubst %.cc, %.o, $(filter %.cc, $(srcs))) \
	$(patsubst %.c, %.o, $(filter %.c, $(srcs))) \
	$(patsubst %.S, %.o, $(filter %.S, $(srcs)))

objs += $(foreach dir, \
			$(submod), \
			$(patsubst %.cc, %.o, $(wildcard $(dir)/*.cc)) \
			)

.PHONY: all clean test

all: kernel 

kernel: $(objs) kernel.ld $(submod)
	$(LD) $(LDFLAGS) -T kernel.ld -o kernel $(objs)

hal:
	$(MAKE) -C hal 

-include *.d

test:
	$(MAKE) test -C sync 
	@echo $(objs)
	@echo $(BUILDPATH)

clean: 
	for mod in $(submod) ; do $(MAKE) clean -C $$mod ; done 
	-rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*.o *.d *.asm *.sym *.out
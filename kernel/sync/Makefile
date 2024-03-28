srcs = $(wildcard *.cc *.c *.S)
subobjs = \
	$(patsubst %.cc, %.o, $(filter %.cc, $(srcs))) \
	$(patsubst %.c, %.o, $(filter %.c, $(srcs))) \
	$(patsubst %.S, %.o, $(filter %.S, $(srcs)))

objs += $(subobjs)

.PHONY: all clean test 

all: $(subobjs)

-include *.d

test:
	@echo $(objs)

clean: 
	-rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*.o *.d *.asm *.sym *.out
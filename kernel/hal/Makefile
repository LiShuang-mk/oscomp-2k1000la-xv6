srcs = $(wildcard *.cc *.c *.S)
objs = \
	$(patsubst %.cc, %.o, $(filter %.cc, $(srcs))) \
	$(patsubst %.c, %.o, $(filter %.c, $(srcs))) \
	$(patsubst %.S, %.o, $(filter %.S, $(srcs)))

.PHONY: all clean 

all:$(objs)
	
-include *.d

clean: 
	-rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*.o *.d *.asm *.sym *.out
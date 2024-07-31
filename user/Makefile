CXXFLAGS += -Iinclude
CFLAGS += -Iinclude
ASFLAGS += -Iinclude

.PHONY: all initdir

c_src = $(wildcard *.c)
cc_src =
s_src = $(wildcard *.S)

srcs = $(c_src) $(cc_src) $(s_src)

c_obj = $(patsubst %.c,$(BUILDPATH)/user/%.o,$(c_src))
cc_obj = $(patsubst %.cc,$(BUILDPATH)/user/%.o,$(cc_src))
s_obj = $(patsubst %.S,$(BUILDPATH)/user/%.o,$(s_src))
objs = $(c_obj) $(cc_obj) $(s_obj)

depends = \
	$(patsubst %.cc, $(BUILDPATH)/user/%.d, $(cc_src)) \
	$(patsubst %.c, $(BUILDPATH)/user/%.d, $(c_src)) \
	$(patsubst %.S, $(BUILDPATH)/user/%.d, $(s_src))

all: initdir $(BUILDPATH)/user/user.a

$(BUILDPATH)/user/user.a: $(objs)
	$(AR) rcs $@ $^

$(cc_obj): $(BUILDPATH)/user/%.o: %.cc 
	$(CXX) -c $(CXXFLAGS) -MF $(BUILDPATH)/user/$*.d -o $@ $<

$(c_obj): $(BUILDPATH)/user/%.o: %.c 
	$(CC) -c $(CFLAGS) -MF $(BUILDPATH)/user/$*.d -o $@ $<

$(s_obj): $(BUILDPATH)/user/%.o: %.S 
	$(CC) -c $(ASFLAGS) -MF $(BUILDPATH)/user/$*.d -o $@ $<

-include $(depends)

initdir:
	mkdir -p $(BUILDPATH)/user

clean:
	cd $(BUILDPATH)/user; rm -f user.a *.o *.d

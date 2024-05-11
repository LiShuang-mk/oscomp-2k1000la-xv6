CXXFLAGS += -I include 
CXXFLAGS += -I test/packages/EABase/include/Common
CXXFLAGS += -I ../../kernel/include/
CXXFLAGS += -I ../../kernel/include/klib
CXXFLAGS += -Wno-deprecated-declarations

#cc_src = $(filter-out source/thread_support.cpp, $(wildcard source/*.cpp))
cc_src = $(wildcard source/*.cpp)
cc_obj = $(patsubst %.cpp,$(BUILDPATH)/thirdparty/EASTL/%.o,$(cc_src))



.PHONY: all clean initdir test

all: $(BUILDPATH)/thirdparty/EASTL/libeastl.a
	@echo "libeastl.a is created..."

initdir:
	mkdir -p $(BUILDPATH)/thirdparty/EASTL
	mkdir -p $(BUILDPATH)/thirdparty/EASTL/source

$(BUILDPATH)/thirdparty/EASTL/libeastl.a: $(cc_obj)
	$(AR) rcs $@ $^

$(BUILDPATH)/thirdparty/EASTL/source/%.o: source/%.cpp
	$(CXX) -c $(CXXFLAGS) -MF $(BUILDPATH)/thirdparty/EASTL/source/$*.d -o $@ $<
clean:
	rm -rf $(BUILDPATH)/thirdparty/EASTL

test:	
	@echo $(cc_src)
	@echo $(cc_obj)
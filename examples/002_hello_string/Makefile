EXTRA_CXX ?= g++
EXTRA_CFLAGS ?= -Wall -Wextra -Werror -Ofast -fno-exceptions -fno-rtti -flto
EXTRA_LDFLAGS ?= -flto

VPATH := .
VPATH += ../../include/tvm
VPATH += ../../include/tvm/library
VPATH += ../../include/tvm/converter
VPATH += ../../thirdparty/concurrentqueue
VPATH += ../../thirdparty/json/include

HDR := $(foreach sdir,$(VPATH),$(wildcard $(sdir)/*.h))

CFLAGS += --std=c++17 $(EXTRA_CFLAGS)
CFLAGS += -I../../include
CFLAGS += -I../../thirdparty/concurrentqueue
CFLAGS += -I../../thirdparty/json/include

LDFLAGS += -lpthread $(EXTRA_LDFLAGS)

all: build/engine build/converter

build/%.o: %.cpp $(HDR) Makefile
	@mkdir -p $(@D)
	$(EXTRA_CXX) -c -o $@ $< $(CFLAGS)

build/engine: build/engine.o
	@mkdir -p $(@D)
	$(EXTRA_CXX) -o $@ $^ $(LDFLAGS)

build/converter: build/converter.o
	@mkdir -p $(@D)
	$(EXTRA_CXX) -o $@ $^ $(LDFLAGS)

run:
	./build/converter example.json build/example.tvm
	./build/engine build/example.tvm

clean:
	rm -rf build

.PHONY: all build run clean

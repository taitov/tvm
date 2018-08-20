EXTRA_CXX ?= g++
EXTRA_CFLAGS ?= -Wall -Wextra -Werror -Ofast -fno-exceptions -fno-rtti -flto
EXTRA_LDFLAGS ?= -flto
EXTRA_FGREP ?= fgrep

SRCDIRS := .

VPATH := .
VPATH += ../../../include/tvm
VPATH += ../../../include/tvm/library
VPATH += ../../../include/tvm/converter

SRC := $(foreach sdir,$(SRCDIRS),$(wildcard $(sdir)/*.cpp))
HDR := $(foreach sdir,$(VPATH),$(wildcard $(sdir)/*.h))
OBJ := $(SRC:%.cpp=%.o)
CFLAGS := $(addprefix -I,$(VPATH))

CFLAGS += --std=c++17 $(EXTRA_CFLAGS)
CFLAGS += -I../../../include

LDFLAGS += -lpthread $(EXTRA_LDFLAGS)

all: build

build: engine converter

$(OBJ): $(HDR) Makefile

%.o: %.cpp
	$(EXTRA_CXX) -c -o $@ $< $(CFLAGS)

engine: engine.o
	$(EXTRA_CXX) -o $@ $^ $(LDFLAGS)

converter: converter.o
	$(EXTRA_CXX) -o $@ $^ $(LDFLAGS)

run:
	! $(EXTRA_FGREP) -a "_TEXT_ONLY_FOR_CONVERTER_" engine
	! $(EXTRA_FGREP) -a "_TEXT_ONLY_FOR_ENGINE_" converter

.PHONY: clean
clean:
	rm -f *.o
	rm -f engine converter

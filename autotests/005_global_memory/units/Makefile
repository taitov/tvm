BIN_CONVERTER = ../converter
BIN_ENGINE = ../engine

SUBDIRS := $(wildcard */.)

all: run

run: $(SUBDIRS)

$(SUBDIRS):
	$(BIN_CONVERTER) $@/autotest.json $@/autotest.tvm
	$(BIN_ENGINE) $@/autotest.tvm $@/autotest.json

.PHONY: $(SUBDIRS)

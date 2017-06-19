TARGET = step_3_http_server

CC = g++

VPATH := .
VPATH += ../../include/tfvm
VPATH += ../../include/tfvm/library

SRC := $(foreach sdir,$(VPATH),$(wildcard $(sdir)/*.cpp))
HDR := $(foreach sdir,$(VPATH),$(wildcard $(sdir)/*.h))
OBJ := $(SRC:%.cpp=%.o)
CFLAGS := $(addprefix -I,$(VPATH))

BIN = $(TARGET)

CFLAGS += --std=c++14 -Ofast -Wall -Wextra -Werror -Wno-unused-parameter -fno-exceptions
CFLAGS += -I../../include

LDFLAGS += -lpthread

all : $(BIN)

$(OBJ) : $(HDR)

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN) : $(OBJ)
	$(CC) -o $@ $^ $(STATICLIBS) $(LDFLAGS)

.PHONY : clean
clean :
	rm -f $(OBJ)

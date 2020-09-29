# Link to library in client
# g++ -W -Wall -std=c++17 -g -o $(OUT) $(OBJECTS) varint.a
# Reference header file in implementation:
# #include "varint/varint.h"
# -------------------------------------------------------
CXXFLAGS = -W -Wall -pedantic -std=c++17 -g
CXX = g++ ${CXXFLAGS}
LDFLAGS = 
ARFLAGS = -crs
AR = ar ${ARFLAGS}
BIN_DIR:= bin
LIB_DIR:= lib
OBJ_DIR:= obj
OBJECT_FILES = $(CPP_SOURCES:%.cpp=$(OBJ_DIR)/%.o)
$(info OBJECT_FILES = "$(OBJECT_FILES)")

.PHONY: all clean $(DIRS)

all: $(BIN_DIR)/example $(LIB_DIR)/varint.a

$(BIN_DIR)/example: $(OBJ_DIR)/main.o $(OBJ_DIR)/varint.o | $(BIN_DIR)
	@$(CXX) $(LDFLAGS) -o $@ $^

$(LIB_DIR)/varint.a: varint.o | $(LIB_DIR)
	$(AR) $@ $<

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

$(LIB_DIR):
	mkdir -p $@

clean:
	rm $(OBJ_DIR)/*
	rm $(BIN_DIR)/*
	rm $(LIB_DIR)/*

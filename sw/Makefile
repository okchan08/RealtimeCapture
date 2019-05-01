CC = g++
CFLAGS = -std=c++11
LDFLAGS = 
LIBS = 
INCLUDE = -I ./include
SRC_DIR = ./src
OBJ_DIR = ./obj
BIN_DIR = ./bin

SRC_SUF = .cpp
OBJ_SUF = .o
DIP_SUF = .d
BIN_SUF =

TARGET = main

SOURCES = $(shell ls $(SRC_DIR)/*$(SRC_SUF)) 
OBJS = $(subst $(SRC_DIR),$(OBJ_DIR), $(SOURCES:$(SRC_SUF)=$(OBJ_SUF)))
TARGETBIN = $(addprefix $(BIN_DIR)/, $(TARGET))
DEPENDS = $(OBJS:$(OBJ_SUF)=$(DIP_SUF))


all: $(TARGETBIN)

web:
	g++ -std=c++11 -g3 test_loop.cpp -I./include $(shell pkg-config --libs --cflags libwebsockets) -luv



$(TARGETBIN): $(OBJS) $(LIBS)
	@if [ ! -d $(BIN_DIR) ]; \
	    then echo "mkdir -p $(BIN_DIR)"; mkdir -p $(BIN_DIR); \
	fi
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)


$(OBJ_DIR)/%.o: $(SRC_DIR)/%$(SRC_SUF)
	@if [ ! -d $(OBJ_DIR) ]; \
	    then echo "mkdir -p $(OBJ_DIR)"; mkdir -p $(OBJ_DIR); \
	fi
	$(CC) $(CFLAGS) $(INCLUDE) $(LDFLAGS) -o $@ -c $<

clean:
	$(RM) -rf $(OBJ_DIR) $(BIN_DIR) $(DEPENDS)

-include $(DEPENDS)

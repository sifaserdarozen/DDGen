# Configuration parameters chosen ...
DYNAMODB = true
S3 = true
dbg = -g

NAME= ddgen
BIN_DIR= ./bin
SRC_DIR= ./src
UTEST_DIR= /tests
LIB_DIR= ./lib
INCLUDE_DIR= ./include
MAKE_DIR= ./tmp
INCLUDE= -I$(INCLUDE_DIR) -I./3rdParty/catch -I./3rdParty/libhttp/include
LINK_LIBRARIES= -lpthread

3RD_PARTY_DIRS = 3rdParty/libhttp
3RD_PARTY_BUILD_DIRS = $(3RD_PARTY_DIRS:%=build-%)
3RD_PARTY_CLEAN_DIRS = $(3RD_PARTY_DIRS:%=clean-%)
LINK_LIBRARIES+= -lhttp -ldl

CPP_OPTIONS= -Wall -g -std=c++14

# to elimanete catch warning
CPP_OPTIONS+= -Wno-unknown-pragmas

# to enable position independent executable for alpine
CPP_OPTIONS+= -fPIC

ifdef gprof
CPP_OPTIONS+= -pg
endif

LINK_OPTIONS= -L./3rdParty/libhttp/lib

ifneq ($(or $(DYNAMODB), $(S3)), )
3RD_PARTY_DIRS+= 3rdParty/aws-sdk/_build
INCLUDE+= -I./3rdParty/aws-sdk/aws-sdk-cpp-1.7.267/aws-cpp-sdk-core/include
LINK_OPTIONS+= -L./3rdParty/aws-sdk/_build/aws-cpp-sdk-core -L./3rdParty/aws-sdk/_build/.deps/install/lib
endif

ifeq ($(DYNAMODB), true)
CPP_OPTIONS+= -DDBASE=DynamoDB
INCLUDE+= -I./3rdParty/aws-sdk/aws-sdk-cpp-1.7.267/aws-cpp-sdk-dynamodb/include
LINK_OPTIONS+= -L./3rdParty/aws-sdk/_build/aws-cpp-sdk-dynamodb
LINK_LIBRARIES+= -laws-cpp-sdk-dynamodb
endif

ifeq ($(S3), true)
CPP_OPTIONS+= -DSTORAGE=S3
INCLUDE+= -I./3rdParty/aws-sdk/aws-sdk-cpp-1.7.267/aws-cpp-sdk-s3/include
LINK_OPTIONS+= -L./3rdParty/aws-sdk/_build/aws-cpp-sdk-s3
LINK_LIBRARIES+= -laws-cpp-sdk-s3
endif

ifneq ($(or $(DYNAMODB), $(S3)), )
LINK_LIBRARIES+= -laws-cpp-sdk-core -laws-c-event-stream -laws-checksums -laws-c-common -lcurl -lssl -lcrypto
endif

OPERATING_SYSTEM= $(shell uname -o)

ifeq ($(OPERATING_SYSTEM), GNU/Linux)
CPP_COMPILER= g++
else ifeq ($(OPERATING_SYSTEM), FreeBSD)
CPP_COMPILER= g++47
else
CPP_COMPILER= g++
endif

EXECUTABLES= $(NAME) $(NAME)_utests

TARGETS= $(EXECUTABLES)

vpath %.cpp $(SRC_DIR)
vpath %.h $(INCLUDE_DIR)

ALL_CPP_OBJS= $(patsubst $(SRC_DIR)/%.cpp, $(LIB_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
DISCARD_OBJS= $(patsubst %, $(LIB_DIR)/%.o, $(EXECUTABLES))
CPP_OBJS= $(filter-out $(DISCARD_OBJS), $(ALL_CPP_OBJS))

CPP_MAKE_TARGETS= $(patsubst $(SRC_DIR)/%.cpp, $(MAKE_DIR)/%.d, $(wildcard $(SRC_DIR)/*.cpp))

TARGET_LIB= $(LIB_DIR)/$(NAME)_lib.a

.PHONY: link compile clean doc makegen

link: compile $(TARGETS)
	@echo -- Link Ended

clean: $(3RD_PARTY_CLEAN_DIRS)
	rm -f $(BIN_DIR)/*
	rm -f $(MAKE_DIR)/*
	rm -f $(LIB_DIR)/*
	@echo "--> Clean Ended"

doc:
	@echo "--> documentation may be done through"
	@echo "--> doxygen ./doc/Doxyfile"

compile: $(CPP_OBJS) $(CPP_MAKE_TARGETS) $(3RD_PARTY_BUILD_DIRS)
	@echo "Compiling ended for.... " $(CPP_OBJS)

-include $(CPP_MAKE_TARGETS)

$(3RD_PARTY_BUILD_DIRS):
	$(MAKE) -C $(@:build-%=%) SSL_LIB=libssl.so.1.0.0 CRYPTO_LIB=libcrypto.so.1.0.0

$(LIB_DIR)/%.o: $(SRC_DIR)/%.cpp | $(LIB_DIR)
	@echo "Compiling... $@"
	$(CPP_COMPILER) $(CPP_OPTIONS) -c $< $(INCLUDE) -o $@

$(LIB_DIR):
	mkdir -p $(LIB_DIR)

$(TARGETS): $(TARGET_LIB) | $(BIN_DIR)
	$(CPP_COMPILER) $(CPP_OPTIONS) $(LINK_OPTIONS) ./src/$@.cpp $^ $(INCLUDE) -o ./bin/$@ $(LINK_LIBRARIES)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET_LIB): $(CPP_OBJS)
	ar rvs  $(TARGET_LIB) $(CPP_OBJS)

$(3RD_PARTY_CLEAN_DIRS):
	$(MAKE) -C $(@:clean-%=%) clean

makegen: $(CPP_MAKE_TARGETS)
	@echo "Making ended for ..." $(CPP_MAKE_TARGETS)

$(MAKE_DIR)/%.d: $(SRC_DIR)/%.cpp | $(MAKE_DIR)
	@echo "Making..... $@"
	@set -e;\
	rm -f $@;\
	$(CPP_COMPILER) $(CPP_OPTIONS) -MM $(INCLUDE) $< >$@.$$$$;\
	sed 's,\($*\)\.o[ :]*, $(LIB_DIR)/\1.o $@: ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

$(MAKE_DIR):
	mkdir -p $(MAKE_DIR)


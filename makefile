#makefile of capturer

NAME= ddgen
BIN_DIR= ./bin
SRC_DIR= ./src
UTEST_DIR= /tests
LIB_DIR= ./lib
INCLUDE_DIR= ./include
MAKE_DIR= ./tmp
INCLUDE= -I$(INCLUDE_DIR) -I./3rdParty/catch

CPP_OPTIONS= -Wall -g -std=c++11

ifdef gprof
CPP_OPTIONS+= -pg
endif

NVCC_OPTIONS= -g

OPERATING_SYSTEM= $(shell uname -o)

ifeq ($(OPERATING_SYSTEM), GNU/Linux)
CPP_COMPILER= g++
NVCC_COMPILER= nvcc
else ifeq ($(OPERATING_SYSTEM), FreeBSD)
CPP_COMPILER= g++47
NVCC_COMPILER= nvcc
else
CPP_COMPILER= g++
NVCC_COMPILER= nvcc
endif

EXECUTABLES= $(NAME) $(NAME)_utests

TARGETS= $(EXECUTABLES)

vpath %.cpp $(SRC_DIR)
vpath %.cu $(SRC_DIR)
vpath %.h $(INCLUDE_DIR)
vpath %.d $(MAKE_DIR)

ALL_CPP_OBJS= $(patsubst $(SRC_DIR)/%.cpp, $(LIB_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cpp))
DISCARD_OBJS= $(patsubst %, $(LIB_DIR)/%.o, $(EXECUTABLES))
CPP_OBJS= $(filter-out $(DISCARD_OBJS), $(ALL_CPP_OBJS))
CU_OBJS= $(patsubst $(SRC_DIR)/%.cu, $(LIB_DIR)/%.o, $(wildcard $(SRC_DIR)/*.cu))

CPP_MAKE_TARGETS= $(patsubst $(SRC_DIR)/%.cpp, $(MAKE_DIR)/%.d, $(wildcard $(SRC_DIR)/*.cpp))
CU_MAKE_TARGETS= $(patsubst $(SRC_DIR)/%.cu, $(MAKE_DIR)/%.d, $(wildcard $(SRC_DIR)/*.cu))

TARGET_LIB= $(LIB_DIR)/$(NAME)_lib.a

.PHONY: link compile clean doc makegen

link: compile $(TARGETS)
	@echo -- Link Ended

clean:
	rm -f $(BIN_DIR)/*
	rm -f $(MAKE_DIR)/*
	rm -f $(LIB_DIR)/*
	@echo "--> Clean Ended"

doc:
	@echo "--> documentation may be done through"
	@echo "--> doxygen ./doc/Doxyfile"

compile: $(CPP_OBJS) $(CU_OBJS) $(CPP_MAKE_TARGETS) $(CU_MAKE_TARGETS)
	@echo "Compiling ended for.... " $(CPP_OBJS) $(CU_OBJS)

-include $(CPP_MAKE_TARGETS)
-include $(CU_MAKE_TARGETS)

$(LIB_DIR)/%.o: $(SRC_DIR)/%.cpp | $(LIB_DIR)
	@echo "Compiling... $@"
	$(CPP_COMPILER) $(CPP_OPTIONS) -c $< $(INCLUDE) -o $@ 

$(LIB_DIR)/%.o: $(SRC_DIR)/%.cu | $(LIB_DIR)
	@echo "Compiling... $@"	
	$(NVCC_COMPILER) $(NVCC_OPTIONS) -c $< $(INCLUDE) -o $@	

$(LIB_DIR):
	mkdir -p $(LIB_DIR)
    
$(TARGETS): $(TARGET_LIB) $(CU_OBJS) | $(BIN_DIR)
	$(CPP_COMPILER) $(CPP_OPTIONS) ./src/$@.cpp $^ $(INCLUDE) -o ./bin/$@ -lpthread -lcudart
    
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET_LIB): $(CPP_OBJS)
	ar rvs  $(TARGET_LIB) $(CPP_OBJS)

makegen: $(CPP_MAKE_TARGETS) $(CU_MAKE_TARGETS)
	@echo "Making ended for ..." $(CPP_MAKE_TARGETS) $(CU_MAKE_TARGETS)

$(MAKE_DIR)/%.d: $(SRC_DIR)/%.cpp | $(MAKE_DIR)
	@echo "Making..... $@" 
	@set -e;\
	rm -f $@;\
	$(CPP_COMPILER) $(CPP_OPTIONS) -MM $(INCLUDE) $< >$@.$$$$;\
	sed 's,\($*\)\.o[ :]*, $(LIB_DIR)/\1.o $@: ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$
    
$(MAKE_DIR)/%.d: $(SRC_DIR)/%.cu | $(MAKE_DIR)
	@echo "Making..... $@" 
	@set -e;\
	rm -f $@;\
	$(NVCC_COMPILER) $(NVCC_OPTIONS) -M $(INCLUDE) $< >$@.$$$$;\
	sed 's,\($*\)\.o[ :]*, $(BIN)/\1.o $@: ,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

$(MAKE_DIR):
	mkdir -p $(MAKE_DIR)

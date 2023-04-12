# Be quiet per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q		:= @
NULL		:= 2>/dev/null
endif

TEST_BINARY = test_runner

# Project specific path
THIS_MAKFILE_PATH := $(abspath $(lastword $(MAKEFILE_LIST)))
THIS_MAKFILE_DIR := $(patsubst %/,%,$(dir $(THIS_MAKFILE_PATH)))
TOPDIR = $(shell realpath $(THIS_MAKFILE_DIR)/..)
SRC_DIR = $(TOPDIR)/src/TIC
INC_DIR = $(TOPDIR)/include
TEST_SRC_DIR = $(TOPDIR)/test

# Own project sources
SRC_FILES  += $(SRC_DIR)/Unframer.cpp
SRC_FILES  += $(SRC_DIR)/DatasetExtractor.cpp
SRC_FILES  += $(SRC_DIR)/DatasetView.cpp

TEST_SRC_FILES = $(shell find $(TEST_SRC_DIR) -name '*.c' -o -name '*.cpp')

# Project includes
INCLUDES_FILES   = $(INC_DIR)

# Vendor includes
INCLUDES += $(INCLUDES_FILES:%=-I%)

# Compiler Flags
CXXFLAGS  = -g -O0 -Wall -Wextra -Warray-bounds -Wno-unused-parameter -Weffc++
CXXFLAGS += -D__TIC_LIB_USE_STD_STRING__
CXXFLAGS += $(INCLUDES)

# Linker Flags
#LDLIBS   += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group

###############################################################################

OBJS = $(SRC_FILES:.cpp=.o)
TEST_OBJS = $(TEST_SRC_FILES:.cpp=.o)
ALL_OBJS = $(OBJS) $(TEST_OBJS)

.PHONY: clean sanity

all: sanity check

sanity:
	@if ! which realpath >/dev/null; then echo "To execute this makefile, you will need to install the tool realpath from coreutils" >&2; exit 1; fi
	@if ! which $(CC) >/dev/null; then echo "To execute this makefile, you will need to install a C compiler and set variable CC accordingly (default is $(CC))" >&2; exit 1; fi
	@if ! which $(CXX) >/dev/null; then echo "To execute this makefile, you will need to install a C++ compiler and set variable CXX accordingly (default is $(CXX))" >&2; exit 1; fi

# Compilation targets
%.o: %.c
	@echo "  CC      $(shell realpath --relative-to $(TOPDIR) $(*)).c"
	$(Q)$(CC) $(INCLUDES) $(CXXFLAGS) $(CFLAGS) -o $@ -c $<

%.o: %.cpp
	@echo "  CXX     $(shell realpath --relative-to $(TOPDIR) $(*)).cpp"
	$(Q)$(CXX) $(INCLUDES) $(CXXFLAGS) $(CPPFLAGS) -o $@ -c $<

$(TEST_BINARY): $(ALL_OBJS)
	@echo "  LD      $@"
	@mkdir -p $(dir $@)
	$(Q)$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

check: $(TEST_BINARY)
	@echo "Running unit tests"
	./$<
	
# Clean
clean:
	@rm -f $(ALL_OBJS) $(TEST_BINARY)

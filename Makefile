CXX        := g++
CXXFLAGS   := -std=c++17 -Wall -Wextra -Iinclude

# Source files
PROGRAM_SRCS := $(filter-out src/main.cpp, $(wildcard src/*.cpp))
PROGRAM_OBJS := $(patsubst src/%.cpp, build/%.o, $(PROGRAM_SRCS))
PROGRAM_BIN  := build/program

# main object (only used for program)
MAIN_OBJ := build/main.o

# Test source files
TEST_SRCS := $(wildcard tests/*.cc)
TEST_OBJS := $(patsubst tests/%.cc, build/tests/%.o, $(TEST_SRCS))
TEST_BIN  := build/test_runner

# Libraries
PROGRAM_LDLIBS :=
TEST_LDLIBS := -lgtest -lgtest_main -pthread

.PHONY: all run test clean

all: $(PROGRAM_BIN) $(TEST_BIN)

# Build main program
$(PROGRAM_BIN): $(PROGRAM_OBJS) $(MAIN_OBJ)
	mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(PROGRAM_LDLIBS)

# Build test runner (no main.cpp)
$(TEST_BIN): $(TEST_OBJS) $(PROGRAM_OBJS)
	mkdir -p $(dir $@)
	$(CXX) $^ -o $@ $(TEST_LDLIBS)

# Compile object files
build/%.o: src/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/main.o: src/main.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/tests/%.o: tests/%.cc
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run:
	$(MAKE) clean
	$(MAKE) $(PROGRAM_BIN)
	./$(PROGRAM_BIN)

test:
	$(MAKE) clean
	$(MAKE) $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf build

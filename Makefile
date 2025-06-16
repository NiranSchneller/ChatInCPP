CXX        := g++
CXXFLAGS   := -std=c++17 -Wall -Wextra -Iinclude

PROGRAM_SRCS    := $(wildcard src/*.cpp)
PROGRAM_OBJS    := $(patsubst src/%.cpp, build/%.o, $(PROGRAM_SRCS))
PROGRAM_BIN     := build/program

TEST_SRCS       := $(wildcard tests/*.cc)
TEST_OBJS       := $(patsubst tests/%.cc, build/tests/%.o, $(TEST_SRCS))
TEST_BIN        := build/test_runner

PROGRAM_LDLIBS  :=
TEST_LDLIBS     := -lgtest -lgtest_main -pthread

.PHONY: all run test clean

all: $(PROGRAM_BIN) $(TEST_BIN)

$(PROGRAM_BIN): $(PROGRAM_OBJS)
	mkdir -p build
	$(CXX) $^ -o $@ $(PROGRAM_LDLIBS)

$(TEST_BIN): $(TEST_OBJS)
	mkdir -p build/tests
	$(CXX) $^ -o $@ $(TEST_LDLIBS)

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build/tests/%.o: tests/%.cc
	mkdir -p build/tests
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: $(PROGRAM_BIN)
	./$(PROGRAM_BIN)

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf build

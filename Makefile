CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -Iinclude

# Folders
SRC_DIR := src
INC_DIR := include
OUT_DIR := Compiled
EXEC := $(OUT_DIR)/main

# Source and object files
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OUT_DIR)/%.o, $(SRCS))

# Default rule
all: $(EXEC)

# Create executable
$(EXEC): $(OBJS)
	@mkdir -p $(OUT_DIR)
	$(CXX) $(OBJS) -o $(EXEC)

# Compile .cpp files into .o files
$(OUT_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OUT_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OUT_DIR)

# Build and run
run: all
	$(EXEC)

.PHONY: all clean run
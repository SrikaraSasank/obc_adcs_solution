
CXX ?= g++
CXXFLAGS ?= -O2 -Wall -Wextra -Wpedantic -std=c++17
LDFLAGS ?= 
INCLUDES = -Iinclude -Isrc

SRC = src/udp_link.cpp src/adcs.cpp src/main.cpp
BIN_DIR = bin
TARGET = $(BIN_DIR)/adcs_app

all: $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $(SRC) $(LDFLAGS)

clean:
	rm -rf $(BIN_DIR)

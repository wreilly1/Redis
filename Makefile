# ------------------------------------------------------------------------------
# Makefile for Mini Redis-like C++ Server
# ------------------------------------------------------------------------------

# Compiler and flags
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -pedantic
# If you are using threads or need other libraries, you might add:
# CXXFLAGS += -pthread

# Name of the final executable
TARGET   := myredis

# List all .cpp source files here (relative to the Makefile path)
SOURCES  := main.cpp \
            src/Server.cpp \
            src/Database.cpp \
            src/CommandParser.cpp \
            src/Logger.cpp

# Convert each .cpp in SOURCES to a corresponding .o
OBJECTS  := $(SOURCES:.cpp=.o)

# Default rule: build the executable
all: $(TARGET)

# Link all object files into the final binary
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile each .cpp -> .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Convenient "run" target (runs the server on port 6379 by default)
run: $(TARGET)
	./$(TARGET) --port 6379

# Clean up build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Declare targets that are not actual files
.PHONY: all clean run
